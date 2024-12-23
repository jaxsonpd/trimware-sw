# flight-sim-software

This repository contains the software that is used to run on the flight simulator hardware developed and stored in jaxsonpd/flight-sim-hardware. The setup for this project is detailed in [setup](SETUP.md)

## Architecture

This project is comprised of two programs. One to perform communication directly with the hardware and the second which handles communication with msfs2020.

### Testing

This project uses the [unity](https://github.com/ThrowTheSwitch/Unity) test framework to allow unity testing. This is supplemented by the [FFF](https://github.com/meekrosoft/fff) Fake Function Framework used for faking where needed. The tests are located in `./tests/` and use cmake for execution. It can be manually executed from the host directory using:

```bash
cmake -B build tests
cmake --build build --parallel
ctest --test-dir build --output-on-error
```

Or using the provided bash script as follows:

```bash
source run_tests.sh
```
