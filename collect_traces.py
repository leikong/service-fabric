import babeltrace
import os.path
import sys


def collect():
    if len(sys.argv) != 2:
        print('Usage: python3 {} <lttng session directory>'.format(sys.argv[0]))
        return False

    # python API needs to be pointed all the way to the parent directory
    # containing the "metadata" file
    path = os.path.join(sys.argv[1], "ust/uid/1000/64-bit")
    col = babeltrace.TraceCollection()

    # Add the trace provided by the user (LTTng traces always have
    # the 'ctf' format)
    if col.add_trace(path, 'ctf') is None:
        raise RuntimeError('Cannot add trace')

    # Iterate on events
    for event in col.events:
        level = event.name.replace("service_fabric:tracepoint_", "")
        timestamp = None
        task = None
        subtask = None
        text = None

        for key in event.keys():
            #print(key)
            if key == 'timestamp_begin':
                timestamp = event[key] # TODO: Needs to be converted from ticks?
            elif key == 'taskNameField':
                task = event[key]
            elif key == 'eventNameField':
                subtask = event[key]
            elif key == 'dataField':
                text = event[key]

        print("[{}:{}] {}.{}: {}".format(level, timestamp, task, subtask, text))

    return True


if __name__ == '__main__':
    sys.exit(0 if collect() else 1)
