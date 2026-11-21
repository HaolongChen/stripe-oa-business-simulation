# Merchant Ledger

**Time limit:** 60 minutes  
**Language:** C++20 recommended (C++17 is sufficient)  
**Input/output:** standard input / standard output  
**Difficulty:** implementation, state modeling, invariants

This is an original practice assessment, not an actual Stripe question.

## Scenario

You are implementing a small in-memory ledger for a payment platform. Merchants receive card payments. The platform collects a processing fee. Funds may be held, released, refunded, or disputed.

The input is a sequence of commands. Process commands in order and print exactly one result line per command.

Read all four parts before implementing Part 1. Later parts extend the same model.

## Global conventions

- All amounts are non-negative integer **cents**.
- IDs are case-sensitive, non-empty tokens without whitespace.
- A command is valid only if every stated precondition holds.
- A rejected command must leave the entire state unchanged.
- IDs for accounts, payments, holds, refunds, and disputes are globally unique across their respective entity type. Reuse within the same entity type is invalid.
- Commands are atomic: validate all preconditions and calculate all deltas before mutating state.
- Use 64-bit integer arithmetic. Inputs are guaranteed to fit signed 64-bit integers; intermediate multiplication for fee proration may require `__int128`.
- All output is deterministic and must match the formats below exactly.

## Input format

The first line contains integer `N` (`1 <= N <= 200000`), the number of commands.

Each of the next `N` lines contains exactly one command. Tokens are separated by one or more spaces.

## Output format

Print exactly one line per command.

- Successful creation/operation: `OK <TYPE> <id>`
- Successful balance query: `BALANCE <merchantId> <available> <held> <fees>`
- Rejected command: `REJECT`

`TYPE` is one of `ACCOUNT`, `PAYMENT`, `HOLD`, `RELEASE`, `REFUND`, or `DISPUTE`.

No other output is allowed.

---

# Part 1 — Accounts and payments

Implement these commands.

### `ACCOUNT <merchantId>`

Create a merchant account with:
- available balance = 0
- held balance = 0
- fee balance = 0

Reject if the merchant already exists.

Output: `OK ACCOUNT <merchantId>`.

### `CHARGE <paymentId> <merchantId> <gross> <fee>`

Create a payment for an existing merchant.

Preconditions:
- `paymentId` does not already exist.
- `gross > 0`.
- `0 <= fee <= gross`.

Effects:
- merchant available balance increases by `gross - fee`.
- merchant fee balance increases by `fee`.
- payment records its merchant, original gross, original fee, net proceeds, cumulative refunded gross = 0, cumulative refunded fee = 0.

Output: `OK PAYMENT <paymentId>`.

### `BALANCE <merchantId>`

Query an existing merchant.

Output:
`BALANCE <merchantId> <available> <held> <fees>`

At this stage, held and fees are both zero unless changed by later parts.

Reject if the merchant does not exist.

---

# Part 2 — Holds and releases

Add these commands.

### `HOLD <holdId> <merchantId> <amount>`

Create a hold against a merchant's available balance.

Preconditions:
- `holdId` has not been used by another hold.
- merchant exists.
- `amount > 0`.
- merchant available balance is at least `amount`.

Effects:
- available decreases by `amount`.
- held increases by `amount`.
- store the hold with status `ACTIVE` and its merchant and amount.

Output: `OK HOLD <holdId>`.

### `RELEASE <holdId>`

Release an active hold.

Preconditions:
- hold exists.
- hold status is `ACTIVE`.

Effects:
- held decreases by the hold amount.
- available increases by the hold amount.
- hold status becomes `RELEASED`.

Output: `OK RELEASE <holdId>`.

A released hold cannot be released again.

---

# Part 3 — Partial refunds and idempotency

Add this command.

### `REFUND <refundId> <paymentId> <amount>`

Refund part of an original payment.

Preconditions:
- `refundId` has not been used by another refund.
- payment exists.
- payment has no dispute in `OPEN` state.
- `amount > 0`.
- cumulative refunded gross plus `amount` does not exceed the payment's original gross.
- merchant available balance is at least the merchant-side refund debit defined below.

Fee proration:
- The original fee is refunded proportionally to cumulative refunded gross.
- Let `newRefundedGross = oldRefundedGross + amount`.
- Let `targetRefundedFee = floor(originalFee * newRefundedGross / originalGross)`.
- This refund's fee component is `targetRefundedFee - oldRefundedFee`.

Merchant-side debit:
- `merchantDebit = amount - feeComponent`.
- Decrease merchant available balance by `merchantDebit`.
- Decrease merchant fee balance by `feeComponent`.
- Increase the payment's cumulative refunded gross and fee totals.

The cumulative formula is intentional: do not independently round each refund.

Output: `OK REFUND <refundId>`.

### Idempotency

A refund ID is the idempotency key. If a `REFUND` command reuses an existing refund ID, reject it with `REJECT` and make no state changes, even if its other arguments differ.

A refund ID is reserved only after a refund succeeds. A rejected refund does not consume the ID.

---

# Part 4 — Disputes

Add this command.

### `DISPUTE <disputeId> <paymentId> <OPEN|WIN|LOSE>`

A dispute represents a cardholder challenge to a payment. A payment can have at most one dispute. A dispute ID must be globally unique among disputes.

#### `OPEN`

Preconditions:
- dispute ID is unused.
- payment exists.
- payment has no dispute yet.
- payment is not fully refunded.
- merchant available balance is at least the payment's remaining merchant exposure.

Define:
- `remainingGross = originalGross - refundedGross`
- `remainingFee = originalFee - refundedFee`
- `remainingExposure = remainingGross - remainingFee`

Effects:
- move `remainingExposure` from merchant available to merchant held.
- create dispute in `OPEN` state, linked to payment and merchant, storing the held exposure.

Output: `OK DISPUTE <disputeId>`.

#### `WIN`

Preconditions:
- dispute exists and is `OPEN`.

Effects:
- move the dispute's held exposure back to merchant available.
- mark dispute `WON`.

Output: `OK DISPUTE <disputeId>`.

#### `LOSE`

Preconditions:
- dispute exists and is `OPEN`.

Effects:
- remove the dispute's exposure from merchant held.
- do not restore it to available.
- mark dispute `LOST`.

Output: `OK DISPUTE <disputeId>`.

A dispute can be resolved exactly once. Reopening or resolving a resolved dispute is rejected.

A dispute ID is reserved only when `OPEN` succeeds. `WIN` and `LOSE` refer to that existing dispute ID; they do not create a new dispute.

---

## State invariants

Your implementation should preserve these after every accepted command:

1. `available >= 0`.
2. `held >= 0`.
3. `fees >= 0`.
4. For each payment: `0 <= refundedGross <= originalGross`.
5. For each payment: `0 <= refundedFee <= originalFee`.
6. For each payment: `refundedFee` equals the cumulative proration formula.
7. Every active hold is included in the merchant's held balance.
8. Every open dispute is included in the merchant's held balance.
9. Rejected commands have no side effects.
10. A payment's original gross and fee never change.

## Complexity expectations

With hash maps, each command should be expected `O(1)`. Ordered output is not required because each command emits its result immediately in input order. Total expected complexity: `O(N)` time and `O(A + P + H + R + D)` memory, where the variables denote entity counts.

## Suggested self-authored hidden tests

Do not stop at the public sample. Test at least:

- duplicate account, payment, hold, refund, and dispute IDs;
- charge with fee equal to gross;
- hold exactly equal to available balance;
- hold larger than available balance;
- release unknown and already released holds;
- multiple refunds whose individual fee proration rounds down;
- refund that would exceed original gross;
- refund whose merchant debit exceeds available balance;
- reuse a refund ID after a rejected attempt;
- opening a dispute after partial refunds;
- dispute open with insufficient available funds;
- resolving unknown, open, and already resolved disputes;
- a refund after a dispute has opened;
- maximum input size and large integer values.

The specification does not define a concurrency model: commands are sequential and single-threaded.
