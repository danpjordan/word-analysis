import numpy as np
import matplotlib.pyplot as plt
import os
import json

# COLORS = ["#fd7f6f", "#7eb0d5", "#b2e061", "#bd7ebe", "#ffb55a",
#           "#ffee65", "#beb9db", "#fdcce5", "#8bd3c7", "#b30000",
#           "#7c1158", "#4421af", "#1a53ff", "#0d88e6", "#00b7c7",
#           "#5ad45a", "#8be04e", "#ebdc78"]

COLORS = ["#e60049", "#0bb4ff", "#50e991", "#e6d800", "#9b19f5", "#ffa300", "#dc0ab4", "#b3d4ff", "#00bfa0", "#b30000", "#7c1158", "#4421af", "#1a53ff", "#0d88e6", "#00b7c7", "#5ad45a", "#8be04e", "#ebdc78"]


def genIndividualGraphs(dataFile, outputDirectory, numDataPoints, numValuesInDataPoint):

    for i in range(numDataPoints):
        graphFileName = outputDirectory + "/" + \
            dataFile["dataPoints"][i]["name"] + ".png"

        IND = np.arange(numValuesInDataPoint)
        WIDTH = 0.25

        plt.figure(figsize=(10, 6))

        xvals = dataFile["dataPoints"][i]["y-axis"]

        plt.bar(IND + WIDTH, xvals, WIDTH, color=COLORS[i])

        dataFile["dataPoints"][i]["name"]

        plt.xticks(IND + WIDTH,
                   dataFile["tickLabels"])

        plt.xticks(rotation=45)
        plt.title(dataFile["dataPoints"][i]["name"])

        plt.tight_layout()
        plt.savefig(graphFileName, dpi=200)
        plt.close()


def genCombinedGraph(dataFile, outputDirectory, numDataPoints, numValuesInDataPoint):

    graphFileName = outputDirectory + "/All " + dataFile["name"] + ".png"

    IND = np.arange(numValuesInDataPoint)
    WIDTH = 0.80 / numDataPoints

    plt.figure(figsize=(18, 6))

    barArray = []
    barLabelArray = []

    for i in range(numDataPoints):
        xvals = dataFile["dataPoints"][i]["y-axis"]
        bar = plt.bar(IND + WIDTH * (i), xvals, WIDTH, color=COLORS[i])
        barArray.append(bar)

    for i in range(numDataPoints):
        barLabel = dataFile["dataPoints"][i]["name"]
        barLabelArray.append(barLabel)

    plt.xticks(IND + (WIDTH * numValuesInDataPoint / 3),
               dataFile["tickLabels"])
    plt.xticks(rotation=45)
    plt.legend((barArray), (barLabelArray),
               bbox_to_anchor=(1.0, 1.0), loc='upper left')
    plt.title(dataFile["name"])

    plt.tight_layout()
    plt.savefig(graphFileName, dpi=200)
    plt.close()


def graphsFromJson(jsonFileName, graphsDirectory):

    file = open(jsonFileName)
    dataFile = json.load(file)

    outputDirectory = graphsDirectory + "/" + dataFile["name"]

    try:
        os.mkdir(outputDirectory)
    except OSError:
        pass

    try:
        os.mkdir(graphsDirectory)
    except OSError:
        pass

    NUM_DATA_POINTS = len(dataFile["dataPoints"])

    if (NUM_DATA_POINTS > 18):
        print("Capping number of data points to 18")
        NUM_DATA_POINTS = 18

    NUM_VALUES_IN_DATA_POINT = len(dataFile["tickLabels"])

    genCombinedGraph(dataFile, outputDirectory,
                     NUM_DATA_POINTS, NUM_VALUES_IN_DATA_POINT)

    genIndividualGraphs(dataFile, outputDirectory,
                        NUM_DATA_POINTS, NUM_VALUES_IN_DATA_POINT)


if __name__ == "__main__":

    jsonFolder = "data/json"
    graphsFolder = "data/graphs"

    dataSets = ["thing", "otherthing"]
    

    for i in dataSets:
        print("Generating graphs for " + i)
        jsonFileName = jsonFolder + "/" + i + ".json"
        graphsFromJson(jsonFileName, graphsFolder)
