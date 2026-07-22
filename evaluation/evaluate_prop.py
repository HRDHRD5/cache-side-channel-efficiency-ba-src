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
                    if result["header"][i] in ["N_P", "T_P", "N_T", "T_T", "L_E", "N_R", "T_R", "stride", "bit_count", "training_length", "n_a", "n_b"]:
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

def print_measured_props():
    props = parse_csv("props-prop-measure.csv", ["stat"])["data"]["props"]

    T_P_s = 0
    T_T_s = 0
    T_R_s = 0
    L_E_s = 0

    for row in props:
        T_P_s += float(row["T_P"]) / float(row["N_P"])
        T_T_s += float(row["T_T"]) / float(row["N_T"])
        T_R_s += float(row["T_R"]) / float(row["N_R"])
        L_E_s += float(row["L_E"])

    print(f"T_P: {T_P_s/len(props)}")
    print(f"T_T: {T_T_s/len(props)}")
    print(f"T_R: {T_R_s/len(props)}")
    print(f"L_E: {L_E_s/len(props)}")


if __name__ == "__main__":
    print_measured_props()
