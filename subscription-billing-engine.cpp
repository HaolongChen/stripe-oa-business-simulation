#include <bits/stdc++.h>

#define LL long long

using namespace std;

struct Plan
{
    LL monthlyFee = 0;
};

struct Subscription
{
    string planId;
    string customerId;
};

struct Customer
{
    unordered_map<string, string> subscriptionStatus;
};

struct State
{
    unordered_map<string, Plan> plans;
    unordered_map<string, Subscription> subscriptions;
    unordered_map<string, Customer> customers;
};

bool newPlan(State &state, string planId, LL monthlyFee)
{
    if (monthlyFee < 0)
    {
        return false;
    }
    if (state.plans.contains(planId))
    {
        return false;
    }
    Plan plan;
    plan.monthlyFee = monthlyFee;
    state.plans.insert(make_pair(planId, plan));
    return true;
}

bool subscribe(State &state, string subscriptionId, string customerId, string planId)
{
    if (state.subscriptions.contains(subscriptionId))
    {
        return false;
    }
    auto it = state.plans.find(planId);
    if (it == state.plans.end())
    {
        return false;
    }
    Plan plan = it->second;
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

        if (command == "PLAN")
        {
            string planId;
            LL monthlyFee;
            cin >> planId >> monthlyFee;
            if (newPlan(state, planId, monthlyFee))
            {
                continue;
            }
        }

        cout << "REJECT" << endl;
    }
    return 0;
}