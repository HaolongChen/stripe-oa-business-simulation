#include <bits/stdc++.h>
using namespace std;

using int64 = long long;
using i128 = __int128_t;

// Suggested entity models. Extend or replace these as needed.
struct Account {
    int64 available = 0;
    int64 held = 0;
    int64 fees = 0;
};

struct Payment {
    string merchantId;
    int64 originalGross = 0;
    int64 originalFee = 0;
    int64 refundedGross = 0;
    int64 refundedFee = 0;
    string disputeId;
};

struct Hold {
    string merchantId;
    int64 amount = 0;
    bool active = true;
};

struct Dispute {
    string paymentId;
    string merchantId;
    int64 exposure = 0;
    // Suggested states: OPEN, WON, LOST.
    string status = "OPEN";
};

struct State {
    unordered_map<string, Account> accounts;
    unordered_map<string, Payment> payments;
    unordered_map<string, Hold> holds;
    unordered_set<string> refundIds;
    unordered_map<string, Dispute> disputes;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    State state;

    for (int i = 0; i < n; ++i) {
        string command;
        cin >> command;

        // TODO: Parse command arguments.
        // TODO: Validate every precondition before mutating state.
        // TODO: Implement Parts 1–4 in problem.md.
        // TODO: Print exactly one output line per command.

        // Temporary placeholder to keep the starter compile-ready.
        cout << "REJECT\n";
    }

    return 0;
}
