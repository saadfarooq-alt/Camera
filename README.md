# Camera Eye Tracker

A real-time eye detection program written in C++ using OpenCV. Opens your Mac's camera and draws bounding boxes around detected faces and eyes using Haar cascade classifiers.

---

## Requirements

- macOS (tested on Apple Silicon)
- [OpenCV 4](https://opencv.org/) via Homebrew
- A C++ compiler (Xcode CLT / clang++)

### Install dependencies

```bash
brew install opencv
```

---

## Build

```bash
g++ main.cc -o camera_capture `pkg-config --cflags --libs opencv4`
```

---

## Run

```bash
./camera_capture 1
```

> **Why index `1`?** On MacBooks with Continuity Camera enabled, index `0` is the iPhone camera (which drops immediately without a signed app). Index `1` is the built-in FaceTime camera. Run `./list_cameras` if you're unsure which index to use.

### Find the right camera index

```bash
g++ list_camera.cc -o list_cameras `pkg-config --cflags --libs opencv4`
./list_cameras
```

---

## Controls

| Key | Action |
|-----|--------|
| `Q` / `ESC` | Quit |
| `E` | Toggle eye detection on/off |
| `I` | Print camera info to console |

---

## How it works

Detection runs in two stages per frame:

1. **Face detection** — scans the full frame using `haarcascade_frontalface_default.xml`. Detected faces are outlined in blue.
2. **Eye detection** — within each face region, searches only the top 60% (to avoid false positives from the mouth/nose) using `haarcascade_eye.xml`. Detected eyes are outlined in green with a center dot.

Both cascade files ship with OpenCV and are loaded from:
```
/opt/homebrew/Cellar/opencv/4.13.0_3/share/opencv4/haarcascades/
```

If your OpenCV version differs, update the `HAAR_BASE` path at the top of `main.cc`.

---

## Tips for better detection

- **Good lighting** facing your face makes a big difference
- **Face the camera straight on** — Haar cascades are trained on frontal faces
- If you get lots of false positives, increase the `minNeighbours` parameter in `detectMultiScale`
- If eyes aren't being detected, try moving closer to the camera or improving lighting

---

## Files

| File | Description |
|------|-------------|
| `main.cc` | Main program — camera capture + face/eye detection |
| `list_camera.cc` | Utility to scan and print available camera indices |

---

## What's next?

Some directions to take this further:

- **Pupil tracking** — find the pupil center within each detected eye region using `HoughCircles`
- **Gaze estimation** — estimate where the user is looking on screen
- **Blink detection** — track eye aspect ratio over time to detect blinks
- **Drowsiness detection** — alert when eyes have been closed too long
