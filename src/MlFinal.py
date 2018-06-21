#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Date    : 2018-05-27 10:19:37
# @Author  : 刘建南
# @Link    : 
# @Version : $Id$

import os
import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from datetime import datetime

np.random.seed(1994)

# 图片尺寸：45 x 45
# 经过图片中心提取之后维度为28 x 28
# 训练样本规模：60000个图片
# 测试样本规模：10000个图片

# load data
trainSet = np.fromfile('datas/mnist_train_data_extracted', dtype=np.uint8)
trainLabelVector = np.fromfile('datas/mnist_train_label', dtype=np.uint8)
testSet = np.fromfile('datas/mnist_test_data_extracted',dtype=np.uint8)
testLabelVector = np.fromfile('datas/mnist_test_label',dtype=np.uint8)

def pca_knn(k,n):
	'''
	PCA WITH KNN
	'''
	import KNN
	import decisionTree as dt
	import numpy as np
	global trainSet
	global testSet
	trainSet = trainSet.reshape(60000,28*28)
	testSet = testSet.reshape(10000,28*28)
	trainSet.astype('float32')
	testSet.astype('float32')
	trainSet = trainSet / 255
	testSet = testSet / 255
	print(trainSet.shape)
	print(k)
	pca = PCA(n_components=n)
	pca.fit(trainSet)
	changedTrainData = pca.transform(trainSet)
	changedTestData = pca.transform(testSet)

	print(changedTrainData.shape[1])
	print(changedTestData.shape[1])
	print(labelVector)
	# norm the data
	normedTrainSet,_,_ = KNN.autoNorm(changedTrainData,0,1)
	normedTrainSet,_,_ = KNN.autoNorm(trainSet,0,1)
	normedTestSet,_,_ = KNN.autoNorm(changedTestData,0,1)
	normedTestSet,_,_ = KNN.autoNorm(testSet,0,1)
	errorNum = 0.0
	testSize = len(normedTestSet)
	print(normedTestSet[0])
	print(testSize)

	beginTime = datetime.now()
	# predict
	for i in range(testSize):
		classifyResult = KNN.knn_classify_matt(normedTestSet[i],normedTrainSet,trainLabelVector,k)
		if classifyResult != testLabelVector[i]:
			errorNum += 1

	print("error rate is {}".format(errorNum / testSize))
	endTime = datetime.now()
	print("分类所花时间：",endTime - beginTime)




def cnn():
	from keras.utils import np_utils
	from keras.models import Sequential,load_model
	from keras.layers import Dense,Dropout,MaxPooling2D,BatchNormalization,Flatten,Conv2D,MaxPool2D
	from keras.optimizers import Adam,SGD
	global trainSet
	global testSet
	trainSet = trainSet.reshape(60000,28,28,1)
	testSet = testSet.reshape(10000,28,28,1)
	trainSet.astype('float32')
	testSet.astype('float32')
	trainSet = trainSet / 255
	testSet = testSet / 255
	global trainLabelVector
	global testLabelVector
	trainLabelVector = np_utils.to_categorical(trainLabelVector,10)
	testLabelVector = np_utils.to_categorical(testLabelVector,10)
	model = Sequential()
	# First Conv layer
	model.add(Conv2D(64,kernel_size=(3,3),activation='relu',input_shape=(28,28,1),padding="same"))
	model.add(BatchNormalization())
	model.add(Dropout(0.25))

	# Second Conv layer
	model.add(Conv2D(64,kernel_size=(3,3),activation='relu',border_mode="same"))
	model.add(BatchNormalization())
	model.add(MaxPooling2D(pool_size=(2,2)))
	model.add(Dropout(0.25))

	# Third Conv layer
	model.add(Conv2D(64,kernel_size=(3,3),activation="relu",border_mode="same"))
	model.add(BatchNormalization())
	model.add(MaxPooling2D(pool_size=(2,2)))
	model.add(Dropout(0.25))

    # Fully connected layer
	model.add(Flatten())
	model.add(Dense(128,activation='relu'))
	model.add(BatchNormalization())
	model.add(Dropout(0.25))

	# Output layer
	model.add(Dense(10,activation='softmax'))


	model.compile(optimizer='adagrad', loss='categorical_crossentropy',metrics=['accuracy'])
	history = model.fit(trainSet,trainLabelVector,batch_size=128,nb_epoch=10,verbose=1)
	score = model.evaluate(testSet,testLabelVector,verbose=0)

	#print loss and accuracy
	print('Test loss:', score[0])
	print('Test accuracy:', score[1])

	#show the data in history
	print(history.history.keys())

    
	#summarize history for accuracy 
	plt.subplot(121)
	plt.plot(history.history["acc"])   
	plt.plot(history.history["val_acc"])
	plt.title("Model accuracy")  
	plt.ylabel("accuracy")  
	plt.xlabel("epoch")  
	plt.legend(["train","test"],loc="upper left")  
	
	#summarize history for loss 
	plt.subplot(122)
	plt.plot(history.history["loss"]) 
	plt.plot(history.history["val_loss"])
	plt.title("Model loss")  
	plt.ylabel("loss")  
	plt.xlabel("epoch")  
	plt.legend(["train","test"],loc="upper left")
	plt.show()
	
	# plt.savefig("loss_cr_opt_adam:"+str(score[1])+".jpg") 
	model.save('model_2')



if __name__ == '__main__':
	# pca_knn(10,70)
	# for k in range(44,50,2):
	# 	pca_knn(10,k)
	for k in range(1,5):
		pca_knn(k, 34)

	# cnn()



