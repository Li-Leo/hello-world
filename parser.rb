#!/usr/bin/env python3import smtplib
# -*- coding: utf-8 -*-
import argparse
import sys
def eg_parser():
    parser = argparse.ArgumentParser(description="calculate X to the power of Y")
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-v", "--verbose", action="store_true")
    group.add_argument("-q", "--quiet", action="store_true")
    parser.add_argument("x", type=int, help="the base")
    parser.add_argument("y", type=int, help="the exponent")
    args = parser.parse_args()
    answer = args.x**args.y

    if args.quiet:
        print(answer)
    elif args.verbose:
        print("{} to the power {} equals {}".format(args.x, args.y, answer))
    else:
        print("{}^{} == {}".format(args.x, args.y, answer))


def parse_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("-n", "--name", type=str, default="Leo", help="this save your name")
    parser.add_argument("-a", "--age", type=str, default="100", help="save your age")
    args = parser.parse_args()
    print(args.name)
    print(args.age)

def main(args):
    print(args.name)
    print(args.age)

if __name__ == '__main__':
    # main(parse_arguments(sys.argv[1:]))
    parse_arguments()
    # eg_parser()
    pass
