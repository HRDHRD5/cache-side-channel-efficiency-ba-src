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
                    if result["header"][i] in ["stride", "bit_count", "training_length", "n_a", "n_b"]:
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
    parsed = parse_csv("result.csv", ["n_b", "n_a"])
    data = parsed["data"]
    result = {}

    for n_b, stride_sets in data.items():
        for n_a, measures in stride_sets.items():
            bits_correct = 0
            bits_transfered = 0
            
            for entry in measures:
                if entry["training_length"] <= 0:
                    continue

                for i in range(n_b * n_a):
                    bits_transfered += 1
                    if entry["input"][i] == entry["output"][i]:
                        bits_correct += 1

            result.setdefault(n_b, {})[n_a] = bits_correct / bits_transfered

    return result


def create_bits_and_stride_datapoints2() -> dict[dict[int]]:
    parsed = parse_csv("result.csv", ["n_b", "n_a"])
    data = parsed["data"]
    result = {}

    for n_b, stride_sets in data.items():
        for n_a, measures in stride_sets.items():
            bits_correct = 0
            bits_transfered = 0
            
            for entry in measures:
                if entry["training_length"] <= 0:
                    continue

                # Show the % where the secret is correctly transfered
                bits_transfered += 1
                if entry["input"] == entry["output"]:
                    bits_correct += 1

            result.setdefault(n_b, {})[n_a] = bits_correct / bits_transfered

    return result

def create_bits_and_stride_datapoints3() -> dict[dict[int]]:
    parsed = parse_csv("result.csv", ["n_b", "n_a"])
    data = parsed["data"]
    result = {}

    for n_b, stride_sets in data.items():
        for n_a, measures in stride_sets.items():
            runtime = 0
            runs = len(measures)
            
            for entry in measures:
                if entry["training_length"] <= 0:
                    continue

                # calculate runtim e-3, because its in cycles
                runtime += int(entry["runtime"])

            result.setdefault(n_b, {})[n_a] = runtime

    return result


def print_graphing_data(datapoints: dict):
    result = "\n---points for graph---\n"
    for i, ival in datapoints.items():
        if type(ival) == dict:
            for j,jval in ival.items():
                result += f"{i,j} [{jval}]"
            result += "\n"
        else:
            result += f"{i,ival}"
    print(result)


def evaluate_bits_mixed(datapoints_func: function):
    datapoints = datapoints_func()
    print_graphing_data(datapoints)

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

            ax.bar3d(i, j, 0, 1, 1, jval * 100, color=(r,g,0))

    ax.set_xlabel('n_b')
    plt.gca().invert_xaxis()
    ax.set_ylabel('n_a')
    ax.set_zlabel('Accuracy')
    ax.set_title('Fehlergenauigkeit des Cache Covert Channels abhängig von Stride und Bit Count')

    # Displaying the plot
    plt.show()


def create_troughput_datapoints() -> dict[int]:
    parsed = parse_csv("result.csv", ["n_b", "n_a"])
    data = parsed["data"]
    result = {}

    for n_b, stride_sets in data.items():
        for n_a, measures in stride_sets.items():
            runtime = 0
            runs = len(measures)
            
            for entry in measures:
                if entry["training_length"] <= 0:
                    continue

                # calculate runtim e-3, because its in cycles
                runtime += int(entry["runtime"])

            runtime = runtime/runs

            # calc throughput as number_of_bits/runtime
            print(f"Bit count {n_b*n_a}")
            print(f"Runtime {runtime}")
            result.setdefault(n_b, {})[n_a] = (n_b*n_a)/runtime

    return result


if __name__ == "__main__":
    evaluate_bits_mixed(create_bits_and_stride_datapoints)
    evaluate_bits_mixed(create_bits_and_stride_datapoints2)
    evaluate_bits_mixed(create_bits_and_stride_datapoints3)
    evaluate_bits_mixed(create_troughput_datapoints)
