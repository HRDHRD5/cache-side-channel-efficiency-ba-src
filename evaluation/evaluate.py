import matplotlib.pyplot as plt
from matplotlib import colors
import math


def parse_csv(file: str, keys: list[str]) -> dict:
    result = {}
    with open(file, "r") as f:
        while line := f.readline():
            if result == {}:
                result["header"] = line.replace("\n", "").split(";")
                result["data"] = {}
            else:
                entry = {}
                for i, column in enumerate(line.replace("\n", "").split(";")):
                    if result["header"][i] in ["stride", "bit_count", "training_length"]:
                        column = int(column)
                    entry[result["header"][i]] = column
                data = result["data"]
                for i, key in enumerate(keys, start=1):
                    if i < len(keys):
                        data.setdefault(entry[key], {})
                    else:
                        data.setdefault(entry[key], []).append(entry)
                    data = data[entry[key]]
    return result


def create_bits_and_stride_datapoints() -> dict[dict[int]]:
    parsed = parse_csv("result.csv", ["stride", "bit_count"])
    data = parsed["data"]
    result = {}

    for stride, stride_sets in data.items():
        for bit_count, measures in stride_sets.items():
            bits_correct = 0
            bits_transfered = 0
            
            for entry in measures:
                for i in range(bit_count):
                    bits_transfered += 1
                    if entry["input"][i] == entry["output"][i]:
                        bits_correct += 1

            result.setdefault(stride, {})[bit_count] = bits_correct / bits_transfered

    return result


def evaluate_bits_and_stride():
    datapoints = create_bits_and_stride_datapoints()

    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    for i, ival in datapoints.items():
        for j,jval in ival.items():
            r = 2-((jval*2))
            g = (jval*2)-1

            r = 1 if r > 1 else r
            r = 0 if r < 0 else r
            g = 1 if g > 1 else g
            g = 0 if g < 0 else g

            ax.bar3d(math.log2(i), j, 0, 1, 1, jval * 100, color=(r,g,0))

    ax.set_xlabel('Stride')
    plt.gca().invert_xaxis()
    ax.set_ylabel('Bit Count')
    ax.set_zlabel('Accuracy')
    ax.set_title('Fehlergenauigkeit des Cache Covert Channels abhängig von Stride und Bit Count')

    # Displaying the plot
    plt.show()


def create_training_length_datapoints() -> dict[int]:
    parsed = parse_csv("result.csv", ["training_length"])
    data = parsed["data"]
    result = {}

    for length, measures in data.items():
        bits_correct = 0
        bits_transfered = 0
        for entry in measures:
            if entry["bit_count"] > 8 or entry["stride"] < 1024:
                continue
            for i in range(entry["bit_count"]):
                bits_transfered += 1
                if entry["input"][i] == entry["output"][i]:
                    bits_correct += 1

        result[length] = bits_correct / bits_transfered

    return result


def evaluate_training_length():
    datapoints = create_training_length_datapoints()

    fig = plt.figure()
    ax = fig.add_subplot(111)

    for length, value in datapoints.items():
        ax.bar(length, value, 1, 0, color=colors.BASE_COLORS["g"])

    ax.set_xlabel('Training Length')
    ax.set_ylabel('Accuracy')
    ax.set_title('Fehlergenauigkeit des Cache Covert Channels abhängig von der Trainingslänge')

    # Displaying the plot
    plt.show()


if __name__ == "__main__":
    evaluate_training_length()
    evaluate_bits_and_stride()
