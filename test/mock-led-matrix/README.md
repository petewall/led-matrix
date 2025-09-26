# Mock LED Matrix Server

This Express server mimics the device firmware endpoints so the web interface can be exercised locally.

## Usage

```bash
yarn start-mock-led-matrix
```

The server listens on <http://localhost:4000> by default and serves both the API endpoints and the static files from the `data/` directory. Environment variables such as `MOCK_LED_MATRIX_COLS`, `MOCK_LED_MATRIX_ROWS`, and `MOCK_LED_MATRIX_PORT` can be used to override defaults.
