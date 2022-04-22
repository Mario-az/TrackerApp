# Tracker App
## _Easy and minimal budget tracker_

[![Build Status](https://github.com/ntkme/github-buttons/workflows/build/badge.svg)](https://www.qt.io/)

Tracker is a minimal budget tracking app, with a local database management, your data is stored locally in the documents folder.

## Features
### Features of this program are various.


The program includes a modern and light user interface, various settings can be accessed, history of transaction can be seen, as well as monthly recap, expenses tracking bar, income tracking bar, a settings interface, a form for adding transactions (scheduling and recurring ones) as well as lookup scheduled transactions.

- Add and delete accounts.
- Set a monthly default salary and savings.
- Add and edit transactions.
- Overview of monthly statistics.
- View and add transactions (Scheduled and recurring)
- Settings screen to change defaults.
- Support for multiple currencies.

> A basic internet connection is required at startup to check location time, this avoids data corruption due to bad system clock or due to manual change of the system clock.

# Preview of the software

<img src="https://user-images.githubusercontent.com/100368490/164795549-c174c784-1bcd-41f4-a2d5-ab596392c497.png" width="800">
<img src="https://user-images.githubusercontent.com/100368490/164799975-645a176b-54d6-4991-a2dd-967a924daa2d.png" width="800">
<img src="https://user-images.githubusercontent.com/100368490/164799968-cd79c349-302a-467b-a07a-fdda1b54d536.png" width="800">

# Tech
## Built with Qt 6

- Tracker uses [Qt6 framework](https://www.qt.io/) for the GUI and core application.
- Tracker uses [TimezoneAPI](https://timezoneapi.io/) to find user time & date.
- Tracker uses [Sqlite3](https://www.sqlite.org/index.html) SQL DB Engine to read and write to user database.

# Steps to install

Tracker requires [Qt](https://www.qt.io/) to run.

Step 1 - clone files
```sh
git clone git@github.com:Mario-az/TrackerApp.git
```
Step 2 - Compile using make after generating Makefile
```sh
make clean
```
Step 3 - Sign up and get token ID from [TimezoneAPI](https://timezoneapi.io/)
```sh
cd Path/to/release/or/Debug/Folder
cat > Token.txt
## INSERT TOKEN HERE ##
Ctrl + D to exit
```

# License

GPL license
