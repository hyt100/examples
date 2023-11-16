#!/usr/bin/python3
from math import fabs
import numpy as np

def savepoints(points, outputFile):
    with open(outputFile, 'w', encoding="utf-8") as fd:
        for p in points:
            coordStr = '{} {} {}'.format(p[0], p[1], p[2])
            colorStr = ' 0 1 0'
            fd.write('v {} {} \n'.format(coordStr, colorStr))

# vectorModels: [{'coords':[np.array[x,y,z], ...], 'faces':[[1,2,3,...], ...], 'colors':[[r,g,b], ...]}, ...] 
def saveobj(vectorModels, outputFile):
    with open(outputFile, 'w', encoding="utf-8") as fd:
        # verticle
        for model in vectorModels:
            coords = model['coords']
            colors = []
            if 'colors' in model:
                colors = model['colors']
            for i in range(len(coords)):
                coordStr = '{} {} {}'.format(coords[i][0], coords[i][1], coords[i][2])
                colorStr = ''
                if len(colors) == len(coords):
                    colorStr = '{} {} {}'.format(colors[i][0]/255, colors[i][1]/255, colors[i][2]/255)
                fd.write('v {} {} \n'.format(coordStr, colorStr))
        fd.write('\n')
        # face
        baseIndex = 1
        for model in vectorModels:
            coords = model['coords']
            faces = model['faces']
            for face in faces:
                faceStr = ' '.join(str(baseIndex + x) for x in face)
                fd.write('f {} \n'.format(faceStr))
            baseIndex += len(coords)

def generateCube(w, h, t, color, transform):
    coordsHomo = [
        np.array([-w/2, +h/2, -t/2, 1]),
        np.array([-w/2, -h/2, -t/2, 1]),
        np.array([+w/2, -h/2, -t/2, 1]),
        np.array([+w/2, +h/2, -t/2, 1]),
        np.array([-w/2, +h/2, +t/2, 1]),
        np.array([-w/2, -h/2, +t/2, 1]),
        np.array([+w/2, -h/2, +t/2, 1]),
        np.array([+w/2, +h/2, +t/2, 1])
    ]
    for i in range(len(coordsHomo)):
        coordsHomo[i] = np.dot(transform, coordsHomo[i])
    faces = [
        [0, 3, 2, 1],
        [4, 5, 6, 7],
        [0, 4, 7, 3],
        [1, 2, 6, 5],
        [7, 6, 2, 3],
        [4, 0, 1, 5]
    ]
    colors = [color for i in range(len(coordsHomo))]
    coords = [np.array([p[0], p[1], p[2]]) for p in coordsHomo]
    return [{'coords':coords, 'faces': faces, 'colors': colors}]

def calOBB(points):
    # 去中心化：计算均值，将原始样本减去均值
    mean = np.array([0.0, 0.0, 0.0])
    for p in points:
        mean += p
    mean /= len(points)
    pointsNew = [d - mean for d in points]

    # 构造协方差矩阵 (是一个关于对角线对称的矩阵)
    m00, m10, m11, m20, m21, m22 = 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
    for i in range(len(pointsNew)):
        m00 += pointsNew[i][0] * pointsNew[i][0]
        m10 += pointsNew[i][0] * pointsNew[i][1]
        m11 += pointsNew[i][1] * pointsNew[i][1]
        m20 += pointsNew[i][0] * pointsNew[i][2]
        m21 += pointsNew[i][1] * pointsNew[i][2]
        m22 += pointsNew[i][2] * pointsNew[i][2]
    m00 /= len(points)
    m10 /= len(points)
    m11 /= len(points)
    m20 /= len(points)
    m21 /= len(points)
    m22 /= len(points)
    mat = np.array([[m00, m10, m20], 
                    [m10, m11, m21], 
                    [m20, m21, m22]])
    
    # 计算协方差矩阵的特征值和特征向量 (eigval数组包含了所有的特征值，eigvec数组包含了与前面特征值对应的特征向量)
    # 这里numpy的特征向量要按列取值
    eigval, eigvec = np.linalg.eigh(mat)
    print('value:', eigval, '\nvector:\n', eigvec)

    # 施密特正交化 (我们任选两个方向，然后正交化)
    xAxis = np.array([eigvec[0][0], eigvec[1][0], eigvec[2][0]])
    yAxis = np.array([eigvec[0][1], eigvec[1][1], eigvec[2][1]])
    zAxis = np.cross(xAxis, yAxis)
    yAxis = np.cross(zAxis, xAxis)
    xAxis /= np.linalg.norm(xAxis)
    yAxis /= np.linalg.norm(yAxis)
    zAxis /= np.linalg.norm(zAxis)

    # 计算旋转矩阵
    rotation3x3 = np.array([[xAxis[0], yAxis[0], zAxis[0]], 
                         [xAxis[1], yAxis[1], zAxis[1]], 
                         [xAxis[2], yAxis[2], zAxis[2]]])
    rotation3x3Inv = np.transpose(rotation3x3)

    # 将新样本变换到Local下 (如果是变换原始样本到local下，我们可以先平移-mean大小)
    pointsLocal = []
    for i in range(len(pointsNew)):
        pointsLocal.append(np.dot(rotation3x3Inv, pointsNew[i]))

    # 计算AABB中心和size
    minp, maxp = pointsLocal[0].copy(), pointsLocal[0].copy()
    for i in range(len(pointsLocal)):
        p = pointsLocal[i]
        minp[0] = min(minp[0], p[0])
        maxp[0] = max(maxp[0], p[0])
        minp[1] = min(minp[1], p[1])
        maxp[1] = max(maxp[1], p[1])
        minp[2] = min(minp[2], p[2])
        maxp[2] = max(maxp[2], p[2])
    xLen = maxp[0] - minp[0]
    yLen = maxp[1] - minp[1]
    zLen = maxp[2] - minp[2]
    centerLocal = (minp + maxp) / 2.0
    centerWorld = np.dot(rotation3x3, centerLocal)
    centerWorld = centerWorld + mean
    
    # 计算OBB的worldMatrix (假定我们求得OBB在local坐标系下是一个1x1x1的单位正方体，经过worldMatrix变换后可以得到OBB)
    translation = np.array([[1.0, 0.0, 0.0, centerWorld[0]], 
                            [0.0, 1.0, 0.0, centerWorld[1]], 
                            [0.0, 0.0, 1.0, centerWorld[2]],
                            [0.0, 0.0, 0.0, 1.0]])
    rotation = np.array([[xAxis[0], yAxis[0], zAxis[0], 0.0], 
                         [xAxis[1], yAxis[1], zAxis[1], 0.0], 
                         [xAxis[2], yAxis[2], zAxis[2], 0.0],
                         [0.0, 0.0, 0.0, 1.0]])
    scale = np.array([[xLen, 0.0, 0.0, 0.0], 
                      [0.0, yLen, 0.0, 0.0], 
                      [0.0, 0.0, zLen, 0.0],
                      [0.0, 0.0, 0.0, 1.0]])
    worldMatrix = translation @ rotation @ scale
    return worldMatrix

points = [
    np.array([1.0, -1.0, -4.0]),
    np.array([2.0, -1.0, -0.5]),
    np.array([-3.0, -1.0, 0.5]),
    np.array([-1.0, -1.0, 5.0]),
    np.array([1.0, 3.0, -4.0]),
    np.array([2.0, 3.0, -0.5]),
    np.array([-3.0, 3.0, 0.5]),
    np.array([-1.0, 3.0, 5.0])
]

worldMatrix = calOBB(points)
# print(worldMatrix)
obbWorld = generateCube(1, 1, 1, [255, 0, 0], worldMatrix)
saveobj(obbWorld, 'obb.obj')
savepoints(points, 'points.obj')