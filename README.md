# Tracker App
## _Easy and minimal budget tracker_

Tracker is a minimal budget tracking app, with a local database management, your data is stored locally in the documents folder.

> For the best experience, please install [Montserrat](https://fonts.google.com/specimen/Montserrat) font from google Fonts.

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

## Future updates

TrackerApp will see various improvements in the future, including:

- UI improvements for Windows.
- Dark mode.
- Ability to convert currencies.
- Charts and spending statistics.
- Ability to use an online (server) database and backup account.

# Preview of the software

<img src="https://user-images.githubusercontent.com/100368490/167104210-fd39d782-021d-4095-ae4f-281a9f25af5e.jpg" width="800">
<img src="https://user-images.githubusercontent.com/100368490/167104174-af5baa3f-9c4c-4749-8a5f-57b4234ca6b7.jpg" width="800">
<img src="https://user-images.githubusercontent.com/100368490/167104197-2a6e1ea9-5b8e-4995-99ca-cdf052434209.jpg" width="800">

# Tech
## Built with Qt 6

- Tracker uses [Qt6 framework](https://www.qt.io/) for the GUI and core application.
- Tracker uses [TimezoneAPI](https://timezoneapi.io/) to find user time & date.
- Tracker uses [Sqlite3](https://www.sqlite.org/index.html) SQL DB Engine to read and write to user database.

# Steps to install

Tracker requires [Qt](https://www.qt.io/), and Qt Network and the Qsql module to compile.

Step 1 - clone files
```sh
git clone git@github.com:Mario-az/TrackerApp.git
```
Step 2 - Compile using qt
```
https://doc.qt.io/qt-5/build-sources.html
```
Step 3 - Sign up and get token ID from [TimezoneAPI](https://timezoneapi.io/)
```sh
cd Path/to/release/or/Debug/Folder
```
create a text file name 'token.txt' and insert token from TimezoneAPI

# License

GPL license
