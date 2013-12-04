__author__ = 'stasstels'

#coding = utf-8

import subprocess as sp
import sys
import glob
import os
import pickle


def runFeatureMatcher(args):
    p = sp.Popen(args, stdout=sp.PIPE)
    return p.communicate()[0].strip().split()


def createPath(dirName, num):
    return dirName + "img" + num + ".jpg"


def visitDirectory(dirName, executable, transforms):
    query = createPath(dirName, '0')
    name = dirName.split("/")[-2]
    res = []
    for t in transforms:
        train = createPath(dirName, t[-1])
        res.append([name + os.path.basename(t)] + runFeatureMatcher([executable, query, train, t]))
    return res


if __name__ == "__main__":
    executable = sys.argv[1]
    transformDir = sys.argv[2]
    imageDirs = sys.argv[3:]

    transforms = []
    for t in glob.glob(transformDir + "T*"):
        transforms.append(t)

    res = sum(map(lambda d: visitDirectory(d, executable, transforms), imageDirs), [])
    print res

    with open("./dump", "wb") as o:
        pickle.dump(res, o)








