# Merchant Ledger — C++20 Business Simulation Mock OA

An original, unofficial 60-minute programming assessment designed to train progressive business-rule implementation under time pressure.

> This is an independently authored practice problem. It is not a Stripe interview question, leaked assessment, or representation of Stripe's actual test.

## Goal

Implement a command-driven merchant ledger in **one C++20 source file**. The specification is intentionally progressive: later parts add constraints to the same state model. Read the entire problem before coding.

## Suggested workflow

1. Read `problem.md` completely before starting the timer.
2. Copy `starter.cpp` to `solution.cpp`.
3. Set a 60-minute timer. Do not use AI or look up solutions during a timed attempt.
4. Run the public sample:
   ```bash
   g++ -std=c++20 -O2 -Wall -Wextra -pedantic solution.cpp -o solution
   ./solution < tests/sample.in
   ```
5. Compare output with `tests/sample.out`.
6. After the attempt, use your own additional tests for invalid transitions, duplicate IDs, partial refunds, and dispute resolution.

## Repository layout

- `problem.md` — complete assessment statement and staged requirements.
- `starter.cpp` — minimal compile-ready entry point; no business logic.
- `tests/sample.in` / `tests/sample.out` — public example.
- `scripts/check.sh` — compile and compare the sample output.

## Important

- Money is represented in integer cents.
- All commands are processed in input order.
- Invalid commands must have no side effects.
- Output is exact: no prompts, debug logs, or explanatory text on stdout.
- The public sample is not exhaustive and does not reveal hidden tests.

## Run checker

```bash
bash scripts/check.sh
```

The checker expects `solution.cpp` in the repository root.
