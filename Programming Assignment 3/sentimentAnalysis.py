import string
from sys import argv
from math import log10

class Dictionary(): 
    def __init__(self, train, test):
        self.training = train
        self.test = test

class PreProcess():
    def __init__(self, train, test):
        self.training = train
        self.test = test

class Bayes():
    def __init__(self, word):
        self.word = word
        self.prob = {}
        self.pos = 0
        self.neg = 0

def createDictionary(setType):
    file = open(setType, "r")
    dictionary = []
    for line in file:
        numbersNwords = line.translate(str.maketrans("", "", string.punctuation))
        numbersNwords = (numbersNwords.lower()).split('\t')
        dictionary += numbersNwords[0].split(" ")
    return sorted(list(set(dictionary)))

def createPreProccess(setType, dictionary):
    file = open(setType, "r")
    length = len(dictionary)
    values = []
    for line in file:
        line = line.translate(str.maketrans("", "", string.punctuation))
        line = (line.strip()).split()
        temp = [0] * length
        temp[length - 1] = int(line[-1].strip()) 
        for i in range(length):
            for j in line:
                if j == dictionary[i]:
                    temp[i] = 1
        values.append(temp)
    return values

def calcProbability(dictionary, preProccess):
    nodeMap = {}
    for i, j in enumerate(dictionary):
        node = Bayes(j)
        truetrue = 0
        truefalse = 0
        falsetrue = 0
        falsefalse = 0
        for k in preProccess:
            if int(k[-1]) == 1:
                node.pos += 1
                if int(k[i]) == 0: falsetrue += 1
                else: truetrue += 1
            else:
                node.neg += 1
                if int(k[i]) == 0: falsefalse += 1
                else: truefalse += 1
        node.prob = [[float(float(falsefalse + 1)/float(node.neg + 2)),
					float(float(falsetrue + 1)/float(node.pos + 2))], 
					[float(float(truefalse + 1)/float(node.neg + 2)), 
					float(float(truetrue + 1)/float(node.pos + 2))]]
        nodeMap[j] = node
    return nodeMap

def calcAccuracy(dictionary, bayes, preProccess, train, test):
    predictions = 0
    correct = 0
    posProb = log10(bayes[dictionary[0]].pos / (bayes[dictionary[0]].pos + bayes[dictionary[0]].neg))
    negProb = log10(bayes[dictionary[0]].neg / (bayes[dictionary[0]].pos + bayes[dictionary[0]].neg))
    for i in preProccess:
        predictions += 1
        tempPos = float(posProb)
        tempNeg = float(negProb)
        for j in range(len(dictionary)):
            tempPos += log10(float(bayes[dictionary[j]].prob[int(i[j])][1]))
            tempNeg += log10(float(bayes[dictionary[j]].prob[int(i[j])][0]))
        if (tempPos >= tempNeg) == int(i[-1]):
            correct += 1 
    
    file = open("results.txt", "a+")
    print("=================================")
    print("Trained File:\t", train)
    print("Ran On:\t\t", test)
    print("Accuracy Found:\t", str(round(((correct / predictions) * 100), 2))+'%')
    print("=================================\n")
    file.write("=================================\n")
    file.write("Trained File:\t"+train+"\n")
    file.write("Ran On:\t\t"+test+"\n")
    file.write("Accuracy Found:\t"+str(round(((correct / predictions) * 100), 2))+'%'+"\n")
    file.write("=================================\n\n")

if __name__ == "__main__":
    # To run the file in MobaXterm: 'python3 sentimentAnalysis.py trainingSet.txt testSet.txt'
    # argv[1] is the trainingSet
    # argv[2] is the testSet

    dictionary = Dictionary((createDictionary(argv[1])), (createDictionary(argv[2])))
    dictionary.training.append('classLabel')
    dictionary.test.append('classLabel')
    
    preProcess = PreProcess(createPreProccess(argv[1], dictionary.training), createPreProccess(argv[2], dictionary.test))
    bayes = calcProbability(dictionary.training, preProcess.training)

    calcAccuracy(dictionary.training, bayes, preProcess.training, argv[1], argv[1])
    calcAccuracy(dictionary.training, bayes, preProcess.test, argv[1], argv[2])

    preProcess.training.insert(0, dictionary.training)
    preProcess.test.insert(0, dictionary.test)

    fileTest = open("preprocessed_test.txt", "w+")
    fileTrain = open("preprocessed_train.txt", "w+")
    for line in preProcess.training:
        fileTest.write(str(line)[1:-1]+"\n")
    for line in preProcess.test:
        fileTrain.write(str(line)[1:-1]+"\n")
    fileTest.close()
    fileTrain.close()