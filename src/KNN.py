'''
knn算法：
优点：精度高、对异常值不敏感，无数据输入假定
缺点：计算复杂度高、空间复杂度高
使用数据范围：数值型和标程型

k-近邻算法的一般流程 
(1)收集数据：可以使用任何方法。
(2)准备数据：距离计算所需要的数值，最好是结构化的数据格式。
(3)分析数据：可以使用任何方法。
(4)训练算法：此步驟不适用于1 近邻算法。
(5)测试算法：计算错误率。
(6)使用算法：首先需要输入样本数据和结构化的输出结果，然后运行K-近邻算法判定输
入数据分别属于哪个分类，最后应用对计算出的分类执行后续的处理。
'''

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import operator
import os

# a test set
def generateData():
    dataset = np.array([[1.0,1.1],[1.0,1.0],[0,0],[0,0.1]])
    labels = list('AABB')
    return dataset,labels


def knn_classify_matt(inX,dataSet,labels,k):
    '''
    Manhattan distance for KNN
    four parameters：
    inX——the input sample；
    dataSet——training samples sets；
    labels——label set of training samples ;，
    k——knn parameter
    '''
    dataSetSize = dataSet.shape[0]
    tileMat = np.tile(inX, (dataSetSize,1))
    diffMat = tileMat - dataSet
    distances = diffMat.sum(axis=1)
    sortedDistIndicies = distances.argsort()
    print(distances[0:10])
    print(sortedDistIndicies[10:20])
    classCount={}          
    for i in range(k):
        voteIlabel = labels[sortedDistIndicies[i]]
        classCount[voteIlabel] = classCount.get(voteIlabel,0) + 1
    sortedClassCount = sorted(classCount.items(), key=operator.itemgetter(1), reverse=True)
    # print(sortedClassCount[0][0],'\n')
    return sortedClassCount[0][0]

def knn_classify(inX,dataSet,labels,k):
    '''
    Euclidean distance for KNN
    knn classifier:
    four parameters：
    inX——the input sample；
    dataSet——training samples sets；
    labels——label set of training samples ;，
    k——knn parameter
    '''
    dataSetSize = dataSet.shape[0]                          # samples' size
    tileMat = np.tile(inX, (dataSetSize,1))
    diffMat = np.array(tileMat - dataSet)                  
    sqDiffMat = np.square(diffMat)
    sqDistances = sqDiffMat.sum(axis=1)
    distances = np.sqrt(sqDistances)
    sortedDistIndicies = distances.argsort()
    print(sqDistances[27192])
    print(sortedDistIndicies[0:10])
    classCount={}          
    for i in range(k):
        voteIlabel = labels[sortedDistIndicies[i]]
        classCount[voteIlabel] = classCount.get(voteIlabel,0) + 1
    sortedClassCount = sorted(classCount.items(), key=operator.itemgetter(1), reverse=True)
    # print(sortedClassCount[0][0],'\n')
    return sortedClassCount[0][0]
    
   

def autoNorm(dataset,leftBound=0,rightBound=1):
    '''
    norm the dataset
    dataset : array-like 
    '''
    minVals = np.min(dataset,axis=0)
    maxVals = np.max(dataset,axis=0)
    ranges = maxVals - minVals
    normedDataset = np.zeros(dataset.shape)
    m = dataset.shape[0]
    normedDataset = dataset - np.tile(minVals,(m,1))
    normedDataset = normedDataset / np.tile(ranges,(m,1)) * (rightBound-leftBound)
    return normedDataset,ranges,minVals


def getNumber(x):
    '''
    whether x is a number or else
    '''
    res = []
    for ele in x:
        try:
            _x = float(ele)
            res.append(_x)
        except TypeError as te:
            res.append(ele)
        except ValueError:
            res.append(ele)
    return res



def file2matrix(filename,n_label=-1,split_type='\t'):
    '''
    filename: file path to open;
    n_label: which feature is label, default is -1
    split_type: split tag
    '''
    with open(filename) as fr:
        arrayOfLines = fr.readlines()
        numberOfLines = len(arrayOfLines)                           # file lines
        dimOfLines = 3                                              # feature dimensions
        returnMatrix = np.zeros((numberOfLines,3))                  # return matrix 
        classLabelsVector = []
        for index,line in enumerate(arrayOfLines):
            line = line.strip()
            arrayFromLine = line.split('\t')
            #arrayFromLine = getNumber(arrayFromLine)      
            returnMatrix[index] = arrayFromLine[:3]
            classLabelsVector.append(int(arrayFromLine[n_label]))
        fr.close()
        return returnMatrix,classLabelsVector

def txtFile2Matrix(filename,n_label=-1):
    '''
    load txt file
    '''
    return file2matrix(filename,n_label=n_label,split_type='\t')


def csvFile2atrix(filename,n_label=-1):
    '''
    load csv file
    '''
    return file2matrix(filename,n_label=n_label,split_type=',')

def datingClassTest():
    testSetRate = 0.1                                                               #test set rate
    fileMatrix,labelVetor = txtFile2Matrix('Ch02/datingTestSet.txt',n_label=-1)
    normedDataset,ranges,minVals = autoNorm(fileMatrix)
    dataSize = normedDataset.shape[0]
    testDataSize = int(testSetRate * dataSize)
    errorNum = 0.0
    for i in range(testDataSize):
        classifyResult = knn_classify(normedDataset[i],normedDataset,labelVetor,7)
        print('the classsify result is {0},the real label is {1}'.format(classifyResult,labelVetor[i]))
        if (classifyResult != labelVetor[i]):errorNum += 1.0
        
    print('the error rate is {0}'.format((errorNum / testDataSize)))




def classifyPerson():
    resultList = ['not at all','in small dose','in large doses']
    percentTats = float(input('percentage of time spent playing video games?'))
    ffMiles = float(input('frequence flier miles earned per year?'))
    icecream = float(input('liters of icecream consumed per year?'))
    datingMat,datingLabels = file2matrix('Ch02/datingTestSet2.txt',n_label=-1)
    normedMat,ranges,minVals = autoNorm(datingMat)
    inArr = np.array([ffMiles,percentTats,icecream])
    classifyResult = knn_classify((inArr-minVals)/ranges,normedMat,datingLabels,k=4)
    print('you will probably like this person : {0}'.format(resultList[classifyResult-1]))
    return

def img2vector(filename):
    fr = open(file=filename)
    arrayOfLines = fr.readlines()
    dataSize = len(arrayOfLines)
    nDim = len(arrayOfLines[0].strip())
    imgVec = np.zeros(dataSize * nDim)
    for i in range(dataSize):
        for j in range(nDim):
            line = arrayOfLines[i].strip()
            imgVec[i*nDim+j] = int(line[j])
    fr.close()
        
    return imgVec

def handWritingClassTest():
    trainingFileList = os.listdir('Ch02/trainingDigits/')
    fileSize = len(trainingFileList)
    hwLabels = np.zeros(fileSize)
    trainingDataSet = np.zeros((fileSize,1024))
    for i,file in enumerate(trainingFileList):
        trainingDataSet[i] = img2vector('Ch02/trainingDigits/{0}'.format(file))
        hwLabels[i] = int(file.strip().split('_')[0])
    testImageList = os.listdir('Ch02/testDigits')
    errorCount = 0.0
    testImageLen = len(testImageList)
    for i,t in enumerate(testImageList):
        #imageName = t.split('.')[0]
        imageLabel = int(t.split('_')[0])
        testImageVector = img2vector('Ch02/testDigits/{0}'.format(t))
        classifyResult = knn_classify(testImageVector,trainingDataSet,hwLabels,k=3)
        print('the classifier came back with {0},the label answer is : {1}\t'.format(classifyResult,imageLabel))
        if(imageLabel!=classifyResult):
            print('++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ')
            errorCount+=1
    print('the total error numbers is : {0}'.format(errorCount))
    print('the total error rate is : {}'.format(errorCount/testImageLen))

