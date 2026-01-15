# Convolution as an Algorithmic Speedup (In‑Class Demo)

This repository accompanies an **in‑class interactive demonstration** for an
Applied Algorithms course.

The purpose is **not** to teach advanced signal processing or Fourier analysis.
The purpose is to show how **convolution + FFT** dramatically improves the
efficiency of an otherwise computationally expensive process.

---

## The big idea (no advanced math)

Filtering an audio signal is just:

> sliding a short list of numbers across a long list of numbers  
> multiplying and adding at each position

That operation is **convolution**.

If:
- the signal has **N** samples
- the filter has **M** coefficients (taps)

then naïve convolution does about:

```
N × M  multiply‑adds
```

That quickly becomes slow.

Using the **Fast Fourier Transform (FFT)**, we can compute the *same result* in
about:

```
N log N
```

This project exists to make that speedup **visible and undeniable**.

---

## What the demo does

1. Loads a WAV audio file (mono or stereo → mono)
2. Applies a **low‑pass FIR filter** to reduce hiss
3. Runs the filter in two ways:
   - Naïve time‑domain convolution (O(N·M))
   - FFT‑based overlap‑add convolution (≈ O(N log N))
4. Prints timing results and speedup

Audio quality is secondary. Runtime behavior is the point.

---

## Build

```bash
make
```

---

## Run

```bash
./demo input.wav
```

Outputs:
- `out_naive.wav`
- `out_fft.wav`

Console output reports runtime and speedup.

---

## What students should observe

- With a **short filter**, naïve convolution is acceptable
- With a **long filter**, naïve convolution becomes very slow
- FFT‑based convolution scales much better

This is a **real algorithmic improvement**, not a micro‑optimization.

---

## Teaching focus

Students are **not expected** to understand FFT math.

They *are expected* to understand:
- what convolution is (a sliding weighted sum)
- why the naïve approach repeats work
- how FFT reduces repeated computation

---

## File overview

- `main.cpp` — driver and timing
- `convolve_naive.cpp` — obvious O(N·M) convolution
- `convolve_fft.cpp` — overlap‑add FFT convolution
- `fir.cpp` — FIR filter construction
- `wav.cpp` — minimal WAV I/O

---

## License

Educational use.