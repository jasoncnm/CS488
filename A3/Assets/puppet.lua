
rootnode = gr.node('root')

rootnode:translate(0, 0.0, -10.0)

white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(1, 2, 1)


shoulders = gr.mesh('sphere','shoulders')
torso:add_child(shoulders)
shoulders:set_material(red)
shoulders:translate(0, 2, 0)
shoulders:scale(2.5, 0.22, 0.5)


neck = gr.mesh('sphere', 'neck')
shoulders:add_child(neck)
neck:set_material(white)
neck:translate(0, 4, 0)
neck:scale(0.1, 0.1, 0.1)

neckJoint = gr.joint('neckJoint', {-50, 0, 50}, {-27, 0, 27})
neck:add_child(neckJoint)
neck:scale(2, 3, 4)

head = gr.mesh('sphere', 'head')
neckJoint:add_child(head)
head:set_material(white)
head:translate(0, 0.8, 0)
head:scale(2, 2, 2)

lefteye = gr.mesh('cube', 'lefteye')
head:add_child(lefteye)
lefteye:set_material(blue)
lefteye:translate(-1, 0, 5)
lefteye:scale(0.2,0.2,0.2)

righteye = gr.mesh('cube', 'righteye')
head:add_child(righteye)
righteye:set_material(blue)
righteye:translate(1, 0, 5)
righteye:scale(0.2,0.2,0.2)

shoulders:scale(1/1.6, 2, 2)


leftshoulderjoint = gr.joint('leftshoulderjoint', {0,0,0}, {-180, 0, 180})
shoulders:add_child(leftshoulderjoint)

leftupperarm = gr.mesh('sphere', 'leftupperarm')
leftshoulderjoint:add_child(leftupperarm)
leftupperarm:set_material(white)
leftupperarm:translate(-4.5, -1, 0)
leftupperarm:scale(0.2, 1, 0.4)

leftelbowjoint = gr.joint('leftelbowjoint', {0,0,0}, {-100, 0 , 0})
leftupperarm:add_child(leftelbowjoint)
leftelbowjoint:translate(0, -1.6, 0)
leftelbowjoint:scale(1/0.2, 1/2, 1/0.75)

leftforearm = gr.mesh('sphere', 'leftforearm')
leftelbowjoint:add_child(leftforearm)
leftforearm:set_material(white)
leftforearm:translate(0, -1.1, 0)
leftforearm:scale(0.12, 2, 0.75* 0.7)

lefthand = gr.mesh('sphere', 'lefthand')
leftforearm:add_child(lefthand)
lefthand:set_material(white)
lefthand:scale(1, 0.2, 0.8)
lefthand:translate(0, -1, 0)


rightshoulderjoint = gr.joint('rightshoulderjoint', {0,0,0}, {-180, 0, 180})
shoulders:add_child(rightshoulderjoint)

rightupperarm = gr.mesh('sphere', 'rightupperarm')
rightshoulderjoint:add_child(rightupperarm)
rightupperarm:set_material(white)
rightupperarm:translate(4.5, -1, 0)
rightupperarm:scale(0.2, 1, 0.4)

rightelbowjoint = gr.joint('rightelbowjoint', {0,0,0}, {-100, 0 , 0})
rightupperarm:add_child(rightelbowjoint)
rightelbowjoint:translate(0, -1.6, 0)
rightelbowjoint:scale(1/0.2, 1/2, 1/0.75)

rightforearm = gr.mesh('sphere', 'rightforearm')
rightelbowjoint:add_child(rightforearm)
rightforearm:set_material(white)
rightforearm:translate(0, -1.1, 0)
rightforearm:scale(0.12, 2, 0.75* 0.7)

righthand = gr.mesh('sphere', 'righthand')
rightforearm:add_child(righthand)
righthand:set_material(white)
righthand:scale(1, 0.2, 0.8)
righthand:translate(0, -1, 0)



return rootnode