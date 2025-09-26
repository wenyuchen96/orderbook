# C++ implementation of a limit orderbook.

### Key Features

- **Price-Time Priority:** Orders are matched based on the best price, and then by the time they were received (First-In, First-Out).
- **Supported Order Types:**
  - **Good-Till-Cancel (GTC):** A standard limit order that rests on the book until it is completely filled or cancelled.
  - **Market:** An order to buy or sell immediately at the best available price(s) on the opposite side of the book.
  - **Immediate-Or-Cancel (IOC):** An order that must be executed immediately. Any portion of the order that cannot be filled is cancelled.
  - **Fill-Or-Kill (FOK):** An order that must be executed immediately and in its entirety. If the full quantity cannot be matched, the entire order is cancelled.
- **Core Operations:**
  - **Add:** Add a new order to the book, triggering an immediate matching attempt.
  - **Cancel:** Remove an active order from the book.
  - **Modify:** Change the price or quantity of an existing order (implemented as a cancel-and-add operation).
