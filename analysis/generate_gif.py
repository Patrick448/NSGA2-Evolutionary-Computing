import helpers
import sys
import os


def main(args):
    print("a")
    print(args)
    #helpers.make_movie(args.base_path, args.num_iter, args.step, args.duration)

#receive command line arguments
if __name__ == '__main__':
    #print(sys.argv)
    #os.system("ls")

    helpers.make_movie(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]), float(sys.argv[4]))
