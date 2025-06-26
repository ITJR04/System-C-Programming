#!/usr/bin/python3
import sys, re
import fileinput
import os.path

def     sys_error(s):
    print ("Error: " + s)
    sys.exit(1)

def     print_help():
    print('''Useage: check-bridge.py [options] [filename]
-v:       print lines
-V:       print status after each line
-w:       check number of waiting cars
filename: the output of the program. If missing, read from stdin. 
''')
    sys.exit(1)

def     print_bridge_status():
    global car_direction, direction, n_corssing, n_crossed, max_crossing, eastb_waiting, westb_waiting;

    print("check: max={},{} direction={} n_crossing={} n_crossed={} eastb_waiting={} westb_waiting={}".
            format(max_crossing, max_crossed, direction, n_crossing, n_crossed, eastb_waiting, westb_waiting))

def     my_error(s):
    global n_lines, line, opt_verbose
    if opt_verbose < 1:
        print(line, end='')
    if opt_verbose < 2:
        print_bridge_status()
    sys_error("Line {}: {}".format(n_lines, s))

opt_verbose = 0
opt_checK_waiting = False 
filenames = []
for a in sys.argv[1:]:
    if a == '-h':
        print_help()
    elif a == '-v':
        opt_verbose = 1
    elif a == '-V':
        opt_verbose = 2
    elif a == '-w':
        opt_checK_waiting = True
    elif len(filenames) == 0 and os.path.isfile(a):
        filenames = [ a ]
    else:
        print_help()

pattern = re.compile('(\d+), (east|west).+ is ([awgl])')
cfg_pattern= re.compile('Options: -w(\d+) -e(\d+) -m(\d+) -c(\d+) -t(\d+)')

# car status 
# assume no more than 999 cars, change to other numbers if necesssary
cars = [None] * 1000;
num_trips = [0] * 1000;
max_car_id = -1

max_n_crossing = 0
max_waiting = 0

# configurations
max_crossing =  max_crossed = 0
eastb_waiting = westb_waiting = n_crossing = n_crossed = 0
n_trips = 0
direction = '' 

n_lines = 0
for line in fileinput.input(filenames):
    if opt_verbose >= 1:
        print(line, end='')

    n_lines += 1
    if n_lines == 1:
        m = re.search(cfg_pattern, line)
        if not m:
            sys_error('Did not see the options line.')
        if int(m.group(1)) + int(m.group(2)) > 1000:
            sys_error('Too many cars.')
        max_crossing = int(m.group(3))
        max_crossed = int(m.group(4))
        n_trips = int(m.group(5))
        assert max_crossing > 0
        assert max_crossed > 0
        assert n_trips > 0
        continue

    m = re.search(pattern, line)
    if (m):
        car_id = int(m.group(1), 0)
        car_direction = m.group(2)
        move = m.group(3)

        max_car_id = max(max_car_id, car_id)   

        # number of cars waiting in the same and opposite direction
        if direction == 'west':
            o_waiting = eastb_waiting 
            s_waiting = westb_waiting 
        else:
            o_waiting = westb_waiting 
            s_waiting = eastb_waiting 

        if move == 'a':    # arriving
            if not cars[car_id] is None:
                my_error("car {} cannot arrive.".format(car_id))
            num_trips[car_id] += 1
            cars[car_id] = move

        elif move == 'w':  # waiting
            if cars[car_id] != 'w' and cars[car_id] != 'a':
                my_error("car {} ({}) is not in a state that can wait, e.g., it has not arrived yet.".format(car_id, cars[car_id]))
            if ((direction == car_direction and n_crossing < max_crossing and 
                (n_crossed < max_crossed or o_waiting == 0))
                or (direction != car_direction and n_crossing == 0 and 
                (n_crossed >= max_crossed or s_waiting == 0))):
                my_error("car {} ({}) does not have to wait".format(car_id, cars[car_id]))

            # increment waiting counter if it is not already waiting 
            if cars[car_id] != 'w':
                if car_direction == 'east':
                    eastb_waiting += 1
                else:
                    westb_waiting += 1
            max_waiting = max(max_waiting, eastb_waiting, westb_waiting)
            cars[car_id] = move

        elif move == 'g':  # getting on the bridge
            if direction == '':
                direction = car_direction

            if cars[car_id] != 'w' and cars[car_id] != 'a':
                my_error("car {} ({}) is not in a state that can get on the bridge, e.g., it has not arrived yet.".
                    format(car_id, cars[car_id]))
            if n_crossing > 0 and direction != car_direction:
                my_error("car {} ({}) cannot get on bridge in opposite direction.".format(car_id, cars[car_id]))
            if direction == car_direction and n_crossing >= max_crossing:
                my_error("car {} ({}) cannot get on bridge because {} cars are already on the bridge.".
                    format(car_id, cars[car_id], max_crossing))
            if direction == car_direction and n_crossed >= max_crossed and o_waiting > 0:
                my_error("car {} ({}) cannot get on bridge because cars are waiting at the other end.".
                    format(car_id, cars[car_id]))

            # swithcing direction 
            if n_crossing == 0 and direction != car_direction:
                direction = car_direction
                n_crossed = 0

            if cars[car_id] == 'w': 
                if direction == 'west': 
                    westb_waiting -= 1
                else:
                    eastb_waiting -= 1

            n_crossing += 1
            n_crossed += 1 

            max_n_crossing = max(max_n_crossing, n_crossing)

            cars[car_id] = move

        elif move == 'l':  # leaving
            if cars[car_id] != 'g':
                my_error("car {} is not on bridge.".format(car_id))
            cars[car_id] = None
            n_crossing -= 1

        else:
            my_error("Unknown car movement")

        if opt_verbose >= 2:
            print_bridge_status()

if n_crossing != 0:
    sys_error("{} car(s) is(are) still on the bridge.".format(n_crossing))

if eastb_waiting != 0:
    sys_error("{} eastbound car(s) is(are) still waiting.".format(eastb_waiting))

if westb_waiting != 0:
    sys_error("{} westbound car(s) is(are) still waiting.".format(westb_waiting))

if max_n_crossing != max_crossing:
    sys_error("(only) {} car(s) was(were) on the bridge at the same time".format(max_n_crossing))

if not all(c is None for c in cars):
    sys_error("some cars have not left the bridge yet.")

if not all(c == n_trips for c in num_trips[0:max_car_id+1]):
    sys_error("some cars did not make {} trips.".format(n_trips))

if opt_checK_waiting and max_waiting == 0:
    sys_error("no car was ever waiting.")

sys.exit(0)
