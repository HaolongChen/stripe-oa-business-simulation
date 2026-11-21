#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
using i128 = __int128_t;

// Suggested entity models. Extend or replace these as needed.
struct Account
{
    int64 available = 0;
    int64 held = 0;
    int64 fees = 0;
};

struct Payment
{
    string merchantId;
    int64 originalGross = 0;
    int64 originalFee = 0;
    int64 refundedGross = 0;
    int64 refundedFee = 0;
    string disputeId;
    bool hasDispute = false;
};

struct Hold
{
    string merchantId;
    int64 amount = 0;
    bool active = true;
};

struct Dispute
{
    string paymentId;
    string merchantId;
    int64 exposure = 0;
    // Suggested states: OPEN, WON, LOST.
    string status = "OPEN";
};

struct State
{
    unordered_map<string, Account> accounts;
    unordered_map<string, Payment> payments;
    unordered_map<string, Hold> holds;
    unordered_set<string> refundIds;
    unordered_map<string, Dispute> disputes;
};

struct StateReturn
{
    Account account;
    Payment payment;
    Hold hold;
    Dispute dispute;
    bool accountAvailable = false;
    bool paymentAvailable = false;
    bool holdAvailable = false;
    bool disputeAvailable = false;
};

StateReturn
balanceService(State &state, string merchantId)
{
    auto it = state.accounts.find(merchantId);
    StateReturn res;
    if (it == state.accounts.end())
    {
        return res;
    }
    res.account = it->second;
    res.accountAvailable = true;
    return res;
}

StateReturn getHold(State &state, string holdId)
{
    auto it = state.holds.find(holdId);
    StateReturn res;
    if (it == state.holds.end())
    {
        return res;
    }
    res.hold = it->second;
    res.holdAvailable = true;
    return res;
}

StateReturn getPayment(State &state, string paymentId)
{
    auto it = state.payments.find(paymentId);
    StateReturn res;
    if (it == state.payments.end())
    {
        return res;
    }
    res.payment = it->second;
    res.paymentAvailable = true;
    return res;
}

StateReturn getDispute(State &state, string disputeId)
{
    auto it = state.disputes.find(disputeId);
    StateReturn res;
    if (it == state.disputes.end())
    {
        return res;
    }
    res.dispute = it->second;
    res.disputeAvailable = true;
    return res;
}

bool accountService(State &state, string merchantId)
{
    if (state.accounts.contains(merchantId))
    {
        return false;
    }
    Account newAccount;
    state.accounts.insert(make_pair(merchantId, newAccount));
    return true;
}

bool chargeService(State &state, string paymentId, string merchantId, int64 gross, int64 fee)
{
    StateReturn accountState = balanceService(state, merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    if (state.payments.contains(paymentId))
    {
        return false;
    }
    if (gross <= 0)
    {
        return false;
    }
    if (fee < 0 || fee > gross)
    {
        return false;
    }
    Account account = accountState.account;
    account.available += gross - fee;
    account.fees += fee;
    Payment payment;
    payment.merchantId = merchantId;
    payment.originalFee = fee;
    payment.originalGross = gross;
    state.accounts.at(merchantId) = account;
    state.payments.insert(make_pair(paymentId, payment));
    return true;
}

bool holdService(State &state, string holdId, string merchantId, int64 amount)
{
    if (amount <= 0)
    {
        return false;
    }
    if (state.holds.contains(holdId))
    {
        return false;
    }
    StateReturn accountState = balanceService(state, merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    if (account.available < amount)
    {
        return false;
    }
    account.available -= amount;
    account.held += amount;
    Hold hold;
    hold.amount = amount;
    hold.merchantId = merchantId;
    hold.active = true;
    state.accounts.at(merchantId) = account;
    state.holds.insert(make_pair(holdId, hold));
    return true;
}

bool releaseService(State &state, string holdId)
{
    StateReturn holdState = getHold(state, holdId);
    if (!holdState.holdAvailable)
    {
        return false;
    }
    Hold hold = holdState.hold;
    if (!hold.active)
    {
        return false;
    }
    StateReturn accountState = balanceService(state, hold.merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    account.held -= hold.amount;
    account.available += hold.amount;
    hold.active = false;
    state.accounts.at(hold.merchantId) = account;
    state.holds.at(holdId) = hold;
    return true;
}

bool refundService(State &state, string refundId, string paymentId, int64 amount)
{
    if (state.refundIds.contains(refundId))
    {
        return false;
    }
    if (amount <= 0)
    {
        return false;
    }
    StateReturn paymentState = getPayment(state, paymentId);
    if (!paymentState.paymentAvailable)
    {
        return false;
    }
    Payment payment = paymentState.payment;
    StateReturn disputeState = getDispute(state, payment.disputeId);
    if (disputeState.disputeAvailable)
    {
        if (disputeState.dispute.status == "OPEN")
        {
            return false;
        }
    }
    if (payment.refundedGross + amount > payment.originalGross)
    {
        return false;
    }
    StateReturn accountState = balanceService(state, payment.merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    int64 newRefundedGross = payment.refundedGross + amount;
    i128 feeMulGross = (i128)(payment.originalFee) * (i128)(newRefundedGross);
    int64 targetRefundedFee = feeMulGross / payment.originalGross;
    int64 feeComponent = targetRefundedFee - payment.refundedFee;
    int64 merchantDebit = amount - feeComponent;
    if (account.available < merchantDebit)
    {
        return false;
    }
    account.available -= merchantDebit;
    account.fees -= feeComponent;
    payment.refundedGross = newRefundedGross;
    payment.refundedFee = targetRefundedFee;
    state.accounts.at(payment.merchantId) = account;
    state.payments.at(paymentId) = payment;
    state.refundIds.insert(refundId);
    return true;
}

bool openDispute(State &state, string disputeId, string paymentId)
{
    if (state.disputes.contains(disputeId))
    {
        return false;
    }
    StateReturn paymentState = getPayment(state, paymentId);
    if (!paymentState.paymentAvailable)
    {
        return false;
    }
    Payment payment = paymentState.payment;
    if (payment.hasDispute)
    {
        return false;
    }
    if (payment.originalGross == payment.refundedGross)
    {
        return false;
    }
    StateReturn accountState = balanceService(state, payment.merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    int64 remainingGross = payment.originalGross - payment.refundedGross;
    int64 remainingFee = payment.originalFee - payment.refundedFee;
    int64 remainingExposure = remainingGross - remainingFee;
    if (account.available < remainingExposure)
    {
        return false;
    }
    account.available -= remainingExposure;
    account.held += remainingExposure;
    Dispute dispute;
    dispute.exposure = remainingExposure;
    dispute.status = "OPEN";
    dispute.merchantId = payment.merchantId;
    dispute.paymentId = paymentId;
    state.accounts.at(payment.merchantId) = account;
    state.payments.at(paymentId) = payment;
    state.disputes.insert(make_pair(disputeId, dispute));
    return true;
}

bool winDispute(State &state, string disputeId)
{
    StateReturn disputeState = getDispute(state, disputeId);
    if (!disputeState.disputeAvailable)
    {
        return false;
    }
    Dispute dispute = disputeState.dispute;
    if (dispute.status != "OPEN")
    {
        return false;
    }
    StateReturn accountState = balanceService(state, dispute.merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    account.available += dispute.exposure;
    dispute.status = "WON";
    state.accounts.at(dispute.merchantId) = account;
    state.disputes.at(disputeId) = dispute;
    return true;
}

bool loseDispute(State &state, string disputeId)
{
    StateReturn disputeState = getDispute(state, disputeId);
    if (!disputeState.disputeAvailable)
    {
        return false;
    }
    Dispute dispute = disputeState.dispute;
    if (dispute.status != "OPEN")
    {
        return false;
    }
    StateReturn accountState = balanceService(state, dispute.merchantId);
    if (!accountState.accountAvailable)
    {
        return false;
    }
    Account account = accountState.account;
    if (account.held < dispute.exposure)
    {
        return false;
    }
    account.held -= dispute.exposure;
    dispute.status = "LOST";
    state.accounts.at(dispute.merchantId) = account;
    state.disputes.at(disputeId) = dispute;
    return true;
}

bool disputeService(State &state, string disputeId, string paymentId, string status)
{
    if (status == "OPEN")
    {
        return openDispute(state, disputeId, paymentId);
    }
    if (status == "WIN")
    {
        return winDispute(state, disputeId);
    }
    if (status == "LOSE")
    {
        return loseDispute(state, disputeId);
    }
    return false;
}

void ok(string command)
{
    cout << "OK " << command << " ";
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n))
        return 0;

    State state;

    for (int i = 0; i < n; ++i)
    {
        string command;
        cin >> command;

        if (command == "ACCOUNT")
        {
            string merchantId;
            cin >> merchantId;
            if (accountService(state, merchantId))
            {
                ok(command);
                cout << merchantId << endl;
                continue;
            }
        }
        else if (command == "CHARGE")
        {
            string paymentId, merchantId;
            int64 gross, fee;
            cin >> paymentId >> merchantId >> gross >> fee;
            if (chargeService(state, paymentId, merchantId, gross, fee))
            {
                ok("PAYMENT");
                cout << paymentId << endl;
                continue;
            }
        }
        else if (command == "BALANCE")
        {
            string merchantId;
            cin >> merchantId;
            StateReturn accountState = balanceService(state, merchantId);
            if (accountState.accountAvailable)
            {
                Account account = accountState.account;
                cout << "BALANCE " << merchantId << " " << account.available << " " << account.held << " " << account.fees << endl;
                continue;
            }
        }
        else if (command == "HOLD")
        {
            string holdId, merchantId;
            int64 amount;
            cin >> holdId >> merchantId >> amount;
            if (holdService(state, holdId, merchantId, amount))
            {
                ok(command);
                cout << holdId << endl;
                continue;
            }
        }
        else if (command == "RELEASE")
        {
            string holdId;
            cin >> holdId;
            if (releaseService(state, holdId))
            {
                ok(command);
                cout << holdId << endl;
                continue;
            }
        }
        else if (command == "REFUND")
        {
            string refundId, paymentId;
            int64 amount;
            cin >> refundId >> paymentId >> amount;
            if (refundService(state, refundId, paymentId, amount))
            {
                ok(command);
                cout << refundId << endl;
                continue;
            }
        }
        else if (command == "DISPUTE")
        {
            string disputeId, paymentId, status;
            cin >> disputeId >> paymentId >> status;
            if (disputeService(state, disputeId, paymentId, status))
            {
                ok(command);
                cout << disputeId << endl;
                continue;
            }
        }

        // TODO: Parse command arguments.
        // TODO: Validate every precondition before mutating state.
        // TODO: Implement Parts 1–4 in problem.md.
        // TODO: Print exactly one output line per command.

        // Temporary placeholder to keep the starter compile-ready.
        cout << "REJECT\n";
    }

    return 0;
}
