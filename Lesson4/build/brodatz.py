__author__ = 'stels'

import urllib2
from subprocess import call
from time import sleep


gif = ".gif"
jpg = ".jpg"
brodatz = "http://www.ux.uis.no/~tranden/brodatz/D"
for i in xrange(1, 113):
    print i
    name = str(i)
    url = brodatz + name + gif
    req = urllib2.Request(url)
    try:
        with open(name + gif, "w") as f:
            image = urllib2.urlopen(req)
            for line in image:
                f.write(line)
        call(["convert", name + gif, name + jpg])
        sleep(1)
    except urllib2.HTTPError:
        continue



