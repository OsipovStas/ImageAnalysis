__author__ = 'stasstels'

import numpy as np
import matplotlib.pyplot as plt
import pickle
import sys
import glob


def setLabels(rectangles, ax):
    for r in rectangles:
        height = r.get_height()
        ax.text(r.get_x() + r.get_width() / 2., 1.05 * height, '%d' % int(height),
                ha='center', va='bottom')


def plot(harris, sift, y_label, title, x_labels):
    ind = np.arange(len(harris))
    width = 0.25
    fig, ax = plt.subplots()
    r1 = ax.bar(ind, harris, width, color="blue")
    r2 = ax.bar(ind + width, sift, width, color="black")
    ax.set_ylabel(y_label)
    ax.set_title(title)
    ax.set_xticks(ind + width)
    ax.set_xticklabels(x_labels)
    ax.legend((r1[0], r2[0]), ('Harris', 'SIFT'))
    setLabels(r1, ax)
    setLabels(r2, ax)
    plt.show()


def getHarrisPerImage(data):
    return map(lambda xs: 100 * float(xs[1]), data)


def getSIFTPerImage(data):
    return map(lambda xs: 100 * float(xs[2]), data)


def getImageNames(data):
    return map(lambda xs: xs[0], data)


def getMeanHarrisPerImageClass(data, classes):
    return map(lambda c: 100 * np.mean(map(lambda d: float(d[1]), filter(lambda d: d[0].find(c) != -1, data))), classes)


def getMeanSIFTPerImageClass(data, classes):
    return map(lambda c: 100 * np.mean(map(lambda d: float(d[2]), filter(lambda d: d[0].find(c) != -1, data))), classes)

if __name__ == "__main__":

    transformDir = sys.argv[1]
    imageDirs = sys.argv[2:]

    transforms = []
    for t in glob.glob(transformDir + "T*"):
        transforms.append(t)

    tClasses = map(lambda s: s.split("/")[-1], transforms)
    iClasses = map(lambda s: s.split("/")[-2], imageDirs)

    data = []
    with open("./dump", "rb") as d:
        data = pickle.load(d)

    y_label = "Good match percent"
    titles = ["Images", "Mean By Image classes", "Mean By Transform"]
    harris = [getHarrisPerImage(data), getMeanHarrisPerImageClass(data, iClasses), getMeanHarrisPerImageClass(data, tClasses)]
    sift = [getSIFTPerImage(data), getMeanSIFTPerImageClass(data, iClasses), getMeanSIFTPerImageClass(data, tClasses)]
    labels = [getImageNames(data), iClasses, tClasses]
    map(lambda (h, s, t, l): plot(h, s, y_label, t, l), zip(harris, sift, titles, labels))




