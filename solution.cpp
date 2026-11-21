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
    if (it == state.accounts.end())
    {
        StateReturn res;
        return res;
    }
    StateReturn res;
    res.account = it->second;
    res.accountAvailable = true;
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
                ok(command);
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
                ok(command);
                cout << merchantId << " " << account.available << " " << account.held << " " << account.fees << endl;
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
