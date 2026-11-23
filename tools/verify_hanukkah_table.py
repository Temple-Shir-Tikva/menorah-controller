#!/usr/bin/env python3
"""Check the Hanukkah table in hanukkah_day.cpp against an independent Hebrew-calendar calculation.

Usage:  python3 tools/verify_hanukkah_table.py [first_year] [last_year]
        (defaults to the years the table covers, 2025-2043)

Each table entry is the *evening* the candles are lit: night 1 is the evening before 25 Kislev.
Table encoding: yearOffset = year - 2024, monthOffset = 0 (Nov) / 1 (Dec) / 2 (Jan), where a
January entry uses January's own year.  No third-party packages are needed.

To extend the table: run this with a larger last_year; it prints every expected night as a
ready-to-paste row for any year the table is missing.
"""
import datetime
import pathlib
import re
import sys

HEBREW_EPOCH = -1373427  # R.D. (Python's date.toordinal() scale) of the Hebrew calendar epoch


def elapsed_days(y):
    months = (235 * y - 234) // 19
    parts = 12084 + 13753 * months
    days = 29 * months + parts // 25920
    return days + 1 if (3 * (days + 1)) % 7 < 3 else days


def new_year_delay(y):
    a, b, c = elapsed_days(y - 1), elapsed_days(y), elapsed_days(y + 1)
    if c - b == 356:
        return 2
    if b - a == 382:
        return 1
    return 0


def rosh_hashana(y):
    return HEBREW_EPOCH + elapsed_days(y) + new_year_delay(y)


def kislev_25(hebrew_year):
    length = rosh_hashana(hebrew_year + 1) - rosh_hashana(hebrew_year)
    long_cheshvan = length in (355, 385)
    return rosh_hashana(hebrew_year) + 30 + (30 if long_cheshvan else 29) + 24


def expected_nights(first_year, last_year):
    nights = {}
    for year in range(first_year, last_year + 1):
        first_candle = datetime.date.fromordinal(kislev_25(year + 3761)) - datetime.timedelta(days=1)
        for n in range(1, 9):
            nights[first_candle + datetime.timedelta(days=n - 1)] = n
    return nights


def table_from_source(path):
    src = path.read_text()
    table = {}
    for year_off, month_off, day, night in re.findall(r"\{(\d+), (\d+), (\d+), (\d+)\}", src):
        month = {0: 11, 1: 12, 2: 1}[int(month_off)]
        table[datetime.date(2024 + int(year_off), month, int(day))] = int(night)
    return table


def row(date, night):
    month_off = {11: 0, 12: 1, 1: 2}[date.month]
    return f"    {{{date.year - 2024}, {month_off}, {date.day}, {night}}},"


def main():
    first = int(sys.argv[1]) if len(sys.argv) > 1 else 2025
    last = int(sys.argv[2]) if len(sys.argv) > 2 else 2043
    source = pathlib.Path(__file__).resolve().parent.parent / "hanukkah_day.cpp"
    table = table_from_source(source)
    expected = expected_nights(first, last)

    problems = 0
    for date in sorted(expected):
        if date not in table:
            print(f"missing  {date}  night {expected[date]}  ->  {row(date, expected[date])}")
            problems += 1
        elif table[date] != expected[date]:
            print(f"WRONG    {date}  table says night {table[date]}, expected {expected[date]}")
            problems += 1
    for date in sorted(table):
        if date not in expected and first <= date.year <= last + 1:
            print(f"EXTRA    {date}  night {table[date]} is not a Hanukkah night")
            problems += 1
    print(f"{len(expected)} nights checked for {first}-{last}: {problems} problem(s)")
    sys.exit(1 if problems else 0)


if __name__ == "__main__":
    main()
