//Servo Spindle

module ForCircle(offset,num) {
    angle = 360 / num;
    for (i = [1:num] )
        {
            rotate([0,0,angle*i])
                translate([0,offset,0])
                    children();
        }
}

module Torus(r1,r2) {
    rotate_extrude(angle = 360, convexity = 2)
        translate([r2,0,0])
            circle(r=r1);
}

module DrillWithFlat(rdrill,height) {
    flat = rdrill*2;
    difference(){
        cylinder(h=height,r=rdrill);
        translate([rdrill*0.8,-flat/2,-1])
            cube([flat,flat,height+2]);
    }
}

module Spindle()
{
rmajor = 12;
hknurlled = 15;
rtorus = 2;
rindent = 3;
rdrill = 3.15;
hflange =4;
difference() {
    union() {
        difference() {
            union() {
                translate([0,0,hknurlled])
                    Torus(rtorus,rmajor-(2*rtorus));
                cylinder(h=hknurlled,r=rmajor);
                cylinder(h=hknurlled+rtorus,r=rmajor-(2*rtorus));
            }
            ForCircle(hknurlled-rindent,23) {
                translate([0,0,-1])
                    cylinder(h=hknurlled + rtorus + 2,r=rindent);
                }
            }
        translate([0,0,-hflange])
            cylinder(h=hflange,r=rmajor*1.2);
        }
    translate([0,0,-hflange-7]) {
        DrillWithFlat(rdrill,hknurlled + rtorus+1 );
    }
    translate([0,0,hknurlled-hflange-9]) {
        cylinder(h=40,r=1.5);
        } 
    }  
            
}

module drillTest() {
    difference() {
        cylinder(h=3,r=8);
        translate([0,0,-15])
            #DrillWithFlat(3.2, 20);
    }
}

$fn = 100;
Spindle();

