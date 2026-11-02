# Evidence-gated stock-3DS compatibility milestone

The evidence checker now runs as part of the existing `make test` Python
unit-test discovery and validates the committed research baseline. Unreported
radio features remain **unknown**, and the baseline must remain explicitly
unverified until independently documented hardware experiments succeed.

Run:

```sh
python3 tools/assess_3ds_compatibility.py research/stock_3ds_evidence.json
python3 tools/assess_3ds_compatibility.py research/stock_3ds_evidence.json --require-verified
```

The second command returns a failing exit code while any capability is
unverified, allowing future CI/release gates to prevent false stock-3DS
compatibility claims. The `demonstrated` label requires a nonempty textual
evidence reference for **every** gate, but the tool cannot authenticate a
laboratory claim: maintainers must manually verify evidence and real-hardware
results.

The milestone is **evidence infrastructure**, not native 3DS StreetPass support.
The original StreetPass Mii Plaza on stock 3DS has not been shown to accept
encounters from StreetPass Mii Plaza DS. No private ROMs, saves, or keys are
included.
