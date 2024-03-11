
rootNode = gr.node('root')
rootNode:translate(0, 0.0, -10.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
brown = gr.material({0.8,0.5, 0.25}, {0.1, 0.1, 0.1}, 10)
darkbrown = gr.material({0.21, 0.14, 0.02}, {0.1, 0.1, 0.1}, 10)
slightDarkbrown = gr.material({0.70, 0.47, 0.23}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0,0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0,1.0, 1.0}, {0.5, 0.5, 0.5}, 10)
yellow = gr.material({1.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
torso:scale(0.8, 0.9, 0.5)
torso:translate(0.0, -0.5, -5.0)
torso:set_material(black)

rootNode:add_child(torso)

neckNode = gr.node('neckNode')
torso:add_child(neckNode)
neckNode:scale(1.0/0.8, 1.0/0.9, 1.0/0.5) 
neckNode:translate(0.0, 0.5, 0.0)
neckJoint = gr.joint('neckJoint', {-20, 0, 70}, {-110, 0, 110});
neckNode:add_child(neckJoint)

head = gr.mesh('sphere', 'head')
head:scale(1.05, 0.9, 0.5)
head:translate(0.0, 0.7, 0.0)
head:set_material(brown)

neckJoint:add_child(head)

leftEye = gr.mesh('sphere', 'leftEye')
leftEye:scale(1/1.0 * 0.08,1/0.9 * 0.08,1/0.5 * 0.01)
leftEye:translate(-0.17, 0.15, 0.98)
leftEye:set_material(black)

head:add_child(leftEye)

rightEye = gr.mesh('sphere', 'rightEye')
rightEye:scale(1/1.0 * 0.08,1/0.9 * 0.08,1/0.5 * 0.01)
rightEye:translate(0.17, 0.15, 0.98)
rightEye:set_material(black)

head:add_child(rightEye)

mouseArea = gr.mesh('sphere', 'mouseArea')
mouseArea:scale(1/1.0 * 0.25,1/0.9 * 0.3,1/0.5 * 0.02)
mouseArea:rotate('x', 10.0)
mouseArea:translate(0, -0.22, 0.95)
mouseArea:set_material(white)

head:add_child(mouseArea)

nose = gr.mesh('sphere', 'nose')
nose:scale(1/1.0 * 0.08,1/0.9 * 0.09,1/0.5 * 0.01)
nose:rotate('x', 10.0)
nose:translate(0, -0.1, 1.0)
nose:set_material(black)

head:add_child(nose)

mouse1 = gr.mesh('cube', 'mouse1')
mouse1:scale(1/1.0 * 0.05,1/0.9 * 0.12,1/0.5 * 0.01)
mouse1:rotate('x', 10.0)
mouse1:translate(0, -0.23, 0.98)
mouse1:set_material(black)

head:add_child(mouse1)

mouse2 = gr.mesh('cube', 'mouse2')
mouse2:scale(1/1.0 * 0.05,1/0.9 * 0.2,1/0.5 * 0.01)
mouse2:rotate('x', 10.0)
mouse2:rotate('z', -30.0)
mouse2:translate(-0.05, -0.35, 0.98)
mouse2:set_material(black)

head:add_child(mouse2)

mouse3 = gr.mesh('cube', 'mouse3')
mouse3:scale(1/1.0 * 0.05,1/0.9 * 0.12,1/0.5 * 0.01)
mouse3:rotate('x', 10.0)
mouse3:rotate('z', 35.0)
mouse3:translate(0.04, -0.33, 0.98)
mouse3:set_material(black)

head:add_child(mouse3)

leftEarNode = gr.node('leftEarNode')
head:add_child(leftEarNode)
leftEarNode:scale(1.0/1, 1.0/0.9, 1.0/0.8) 
leftEarNode:translate(-0.55, 0.65, 0.0)
leftEarJoint = gr.joint('leftEarJoint',  {0, 0, 0},{-30, 0, 30});
leftEarNode:add_child(leftEarJoint)

leftEar = gr.mesh('sphere', 'leftEar')
leftEar:scale(0.32, 0.3, 0.1)
leftEar:set_material(brown)

leftEarJoint:add_child(leftEar)

leftInnerEar = gr.mesh('sphere', 'leftInnerEar')
leftInnerEar:scale(0.8, 0.8, 1)
leftInnerEar:translate(0, 0, 0.2)
leftInnerEar:set_material(darkbrown)

leftEar:add_child(leftInnerEar)

rightEarNode = gr.node('rightEarNode')
head:add_child(rightEarNode)
rightEarNode:scale(1.0/1, 1.0/0.9, 1.0/0.8) 
rightEarNode:translate(0.55, 0.65, 0.0)
rightEarJoint = gr.joint('rightEarJoint',  {0, 0, 0},{-30, 0, 30});
rightEarNode:add_child(rightEarJoint)

rightEar = gr.mesh('sphere', 'rightEar')
rightEar:scale(0.32, 0.3, 0.1)
rightEar:set_material(brown)

rightEarJoint:add_child(rightEar)

rightInnerEar = gr.mesh('sphere', 'rightInnerEar')
rightInnerEar:scale(0.8, 0.8, 1)
rightInnerEar:translate(0, 0, 0.2)
rightInnerEar:set_material(darkbrown)

rightEar:add_child(rightInnerEar)


waistNode = gr.node('waistNode')
torso:add_child(waistNode)
waistNode:scale(1.0/0.8, 1.0/0.9, 1.0/0.5) 
waistNode:translate(0.0, -0.3, 0.0)


waistJoint = gr.joint('waistJoint',  {0, 0, 0},{-30,0, 30});
waistNode:add_child(waistJoint)


waist = gr.mesh('sphere', 'waist')
waist:scale(0.8, 0.5, 0.5)
waist:translate(0.0, -0.4, 0.0)
waist:set_material(slightDarkbrown)

waistJoint:add_child(waist)


leftThighNode = gr.node('leftThighNode')
waist:add_child(leftThighNode)
leftThighNode:scale(1.0/0.8, 1.0/0.5, 1.0/0.5) 
leftThighNode:translate(0.0, -0.3, 0.0)

leftThighJoint = gr.joint('leftThighJoint',  {0, 0, 0},{-60, 0, 60});
leftThighNode:add_child(leftThighJoint)


leftThigh = gr.mesh('sphere', 'leftThigh')
leftThigh:scale(0.3, 0.4, 0.2)
leftThigh:translate(-0.35, -0.4, 0.0)
leftThigh:set_material(red)

leftThighJoint:add_child(leftThigh)


leftShankNode = gr.node('leftShankNode')
leftThigh:add_child(leftShankNode)
leftShankNode:scale(1.0/0.3, 1.0/0.4, 1.0/0.2) 
leftShankNode:translate(0.0, -0.1, 0.0)

leftShankJoint = gr.joint('leftShankJoint',  {0, 0, 0},{-10, 0, 70});
leftShankNode:add_child(leftShankJoint)



leftShank = gr.mesh('cube', 'leftShank')
leftShank:scale(0.3, 0.5, 0.2)
leftShank:translate(0.0, -0.3, 0.0)
leftShank:set_material(red)

leftShankJoint:add_child(leftShank)


leftFootNode = gr.node('leftFootNode')
leftShank:add_child(leftFootNode)
leftFootNode:scale(1.0/0.3, 1.0/0.5, 1.0/0.2) 
leftFootNode:translate(0.0, -0.3, 0.0)


leftFootJoint = gr.joint('leftFootJoint',  {0, 0, 0},{-15, 0, 15});
leftFootNode:add_child(leftFootJoint)



leftFoot = gr.mesh('sphere', 'leftFoot')
leftFoot:scale(0.3, 0.3, 0.2)
leftFoot:translate(0.0, -0.3, 0.0)
leftFoot:set_material(blue)

leftFootJoint:add_child(leftFoot)



rightThighNode = gr.node('rightThighNode')
waist:add_child(rightThighNode)
rightThighNode:scale(1.0/0.8, 1.0/0.5, 1.0/0.5) 
rightThighNode:translate(0.0, -0.3, 0.0)

rightThighJoint = gr.joint('rightThighJoint',  {0, 0, 0},{-60, 0, 60});
rightThighNode:add_child(rightThighJoint)


rightThigh = gr.mesh('sphere', 'rightThigh')
rightThigh:scale(0.3, 0.4, 0.2)
rightThigh:translate(0.35, -0.4, 0.0)
rightThigh:set_material(red)

rightThighJoint:add_child(rightThigh)


rightShankNode = gr.node('rightShankNode')
rightThigh:add_child(rightShankNode)
rightShankNode:scale(1.0/0.3, 1.0/0.4, 1.0/0.2) 
rightShankNode:translate(0.0, -0.1, 0.0)


rightShankJoint = gr.joint('rightShankJoint',  {0, 0, 0},{-10, 0, 70});
rightShankNode:add_child(rightShankJoint)



rightShank = gr.mesh('cube', 'rightShank')
rightShank:scale(0.3, 0.5, 0.2)
rightShank:translate(0.0, -0.3, 0.0)
rightShank:set_material(red)

rightShankJoint:add_child(rightShank)


rightFootNode = gr.node('rightFootNode')
rightShank:add_child(rightFootNode)
rightFootNode:scale(1.0/0.3, 1.0/0.5, 1.0/0.2) 
rightFootNode:translate(0.0, -0.3, 0.0)


rightFootJoint = gr.joint('rightFootJoint',  {0, 0, 0},{-15, 0, 15});
rightFootNode:add_child(rightFootJoint)



rightFoot = gr.mesh('sphere', 'rightFoot')
rightFoot:scale(0.3, 0.3, 0.2)
rightFoot:translate(0.0, -0.3, 0.0)
rightFoot:set_material(blue)

rightFootJoint:add_child(rightFoot)



leftUpperArmNode = gr.node('leftUpperArmNode')
torso:add_child(leftUpperArmNode)
leftUpperArmNode:scale(1.0/0.8, 1.0/0.9, 1.0/0.5) 
leftUpperArmNode:translate(-0.4, 0.0, 0.0)

leftUpperArmJoint = gr.joint('leftUpperArmJoint',  {-30, 0, 30},{-60, 0, 60});
leftUpperArmNode:add_child(leftUpperArmJoint)


leftUpperArm = gr.mesh('sphere', 'leftUpperArm')
leftUpperArm:scale(0.3, 0.4, 0.2)
leftUpperArm:rotate('z', -45.0)
leftUpperArm:translate(-0.4, 0.0, 0.0)
leftUpperArm:set_material(blue)

leftUpperArmJoint:add_child(leftUpperArm)


leftLowerArmNode = gr.node('leftLowerArmNode')
leftUpperArm:add_child(leftLowerArmNode)
leftLowerArmNode:scale(1.0/0.3, 1.0/0.4, 1.0/0.2) 
leftLowerArmNode:translate(0.0, -0.5, 0.0)

leftLowerArmJoint = gr.joint('leftLowerArmJoint',  {0, 0, 0},{-60, 0, 10});
leftLowerArmNode:add_child(leftLowerArmJoint)



leftLowerArm = gr.mesh('cube', 'leftLowerArm')
leftLowerArm:scale(0.3, 0.5, 0.2)
leftLowerArm:translate(0.0, -0.3, 0.0)
leftLowerArm:set_material(black)

leftLowerArmJoint:add_child(leftLowerArm)


leftHandNode = gr.node('leftHandNode')
leftLowerArm:add_child(leftHandNode)
leftHandNode:scale(1.0/0.3, 1.0/0.5, 1.0/0.2) 
leftHandNode:translate(0.0, -0.3, 0.0)


leftHandJoint = gr.joint('leftHandJoint',  {0, 0, 0},{-15, 0, 15});
leftHandNode:add_child(leftHandJoint)



leftHand = gr.mesh('sphere', 'leftHand')
leftHand:scale(0.3, 0.3, 0.2)
leftHand:translate(0.0, -0.2, 0.0)
leftHand:set_material(brown)

leftHandJoint:add_child(leftHand)


rightUpperArmNode = gr.node('rightUpperArmNode')
torso:add_child(rightUpperArmNode)
rightUpperArmNode:scale(1.0/0.8, 1.0/0.9, 1.0/0.5) 
rightUpperArmNode:translate(0.4, 0.0, 0.0)


rightUpperArmJoint = gr.joint('rightUpperArmJoint',  {-30, 0, 30},{-60, 0, 60});
rightUpperArmNode:add_child(rightUpperArmJoint)


rightUpperArm = gr.mesh('sphere', 'rightUpperArm')
rightUpperArm:scale(0.3, 0.4, 0.2)
rightUpperArm:rotate('z', 45.0)
rightUpperArm:translate(0.4, 0.0, 0.0)
rightUpperArm:set_material(blue)

rightUpperArmJoint:add_child(rightUpperArm)


rightLowerArmNode = gr.node('rightLowerArmNode')
rightUpperArm:add_child(rightLowerArmNode)
rightLowerArmNode:scale(1.0/0.3, 1.0/0.4, 1.0/0.2) 
rightLowerArmNode:translate(0.0, -0.5, 0.0)
rightLowerArmJoint = gr.joint('rightLowerArmJoint',  {0, 0, 0},{-60, 0, 10});
rightLowerArmNode:add_child(rightLowerArmJoint)


rightLowerArm = gr.mesh('cube', 'rightLowerArm')
rightLowerArm:scale(0.3, 0.5, 0.2)
rightLowerArm:translate(0.0, -0.3, 0.0)
rightLowerArm:set_material(black)

rightLowerArmJoint:add_child(rightLowerArm)

rightHandNode = gr.node('rightHandNode')
rightLowerArm:add_child(rightHandNode)
rightHandNode:scale(1.0/0.3, 1.0/0.5, 1.0/0.2) 
rightHandNode:translate(0.0, -0.3, 0.0)

rightHandJoint = gr.joint('rightHandJoint',  {0, 0, 0},{-15, 0, 15});
rightHandNode:add_child(rightHandJoint)


rightHand = gr.mesh('sphere', 'rightHand')
rightHand:scale(0.3, 0.3, 0.2)
rightHand:translate(0.0, -0.2, 0.0)
rightHand:set_material(brown)

rightHandJoint:add_child(rightHand)



return rootNode;