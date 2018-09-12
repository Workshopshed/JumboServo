// Copyright 2011 Cliff L. Biffle.
// This file is licensed Creative Commons Attribution-ShareAlike 3.0.
// http://creativecommons.org/licenses/by-sa/3.0/

// You can get this file from http://www.thingiverse.com/thing:3575
use <parametric_involute_gear_v5.0.scad>

// Couple handy arithmetic shortcuts
function sqr(n) = pow(n, 2);
function cube(n) = pow(n, 3);

// This was derived as follows:
// In Greg Frost's original script, the outer radius of a spur
// gear can be computed as...
function gear_outer_radius(number_of_teeth, circular_pitch) =
	(sqr(number_of_teeth) * sqr(circular_pitch) + 64800)
		/ (360 * number_of_teeth * circular_pitch);

// We can fit gears to the spacing by working it backwards.
//  spacing = gear_outer_radius(teeth1, cp)
//          + gear_outer_radius(teeth2, cp);
//
// I plugged this into an algebra system, assuming that spacing,
// teeth1, and teeth2 are given.  By solving for circular pitch,
// we get this terrifying equation:
function fit_spur_gears(n1, n2, spacing) =
	(180 * spacing * n1 * n2  +  180
		* sqrt(-(2*n1*cube(n2)-(sqr(spacing)-4)*sqr(n1)*sqr(n2)+2*cube(n1)*n2)))
	/ (n1*sqr(n2) + sqr(n1)*n2);


module servo_gear() {
    $fn = 100;
	n1 = 38; n2 = 59;
	p = fit_spur_gears(n1, n2, 26.8);
    //p=129.3875;
	// Simple Test:
	/* gear (circular_pitch=p,
		gear_thickness = 3,
		rim_thickness = 3,
		hub_thickness = 3,
	    number_of_teeth = n1,
		circles=8);
	
	translate([gear_outer_radius(n1, p) +   gear_outer_radius(n2, p),0,0])
    */
    
    difference() {
        
    union() {
    //fillet
    difference() {
        cylinder(d=26,h=8);
        translate([0,0,8])
            torus(3,13);
    }    
	gear (circular_pitch=p,
		gear_thickness = 5,
		rim_thickness = 5,
		hub_thickness = 12,
    	hub_diameter=20,
        bore_diameter=0,
		circles=0,
		number_of_teeth = n2,
		rim_width = 1);
    }
        
        
    translate([0,0,4])
        splinegear();
    translate([0,0,11])
        cylinder(d=6,h=10);
        
    }
}

module testgear()
{
        n1 = 38; n2 = 59;
        p = fit_spur_gears(n1, n2, 26.8);
        //  96.4527;
        //p=96.4527;
        //14.7358
        $fn = 100;
        
        echo ("Module:",p);
    difference() {
    	gear (circular_pitch=p,
		gear_thickness = 2,
		rim_thickness = 2,
		hub_thickness = 2,
    	hub_diameter=20,
        bore_diameter=0,
		circles=0,
		number_of_teeth = n2,
		rim_width = 1);
        translate([-7,-5,1.5]) linear_extrude(height=3,convexity = 10, twist = 0) text(text=str(n2));
    }
}

module splinegear() {
    n1 = 38; n2 = 59;
	p = fit_spur_gears(n1, n2, 27);
    gear (circular_pitch=p/1.6,
            gear_thickness = 10,
            rim_thickness = 10,
            hub_thickness = 5,
            bore_diameter=0,
            circles=0,
            number_of_teeth = 19,
            rim_width = 10);
}

module splinetest() {
    $fn = 100;
	n1 = 38; n2 = 59;
	p = fit_spur_gears(n1, n2, 27);
    difference() {
        cylinder(d=10,h=6);
        translate([0,0,2])
            splinegear();
    }
}

module torus(r1,r2) {
    rotate_extrude(angle = 360, convexity = 2)
        translate([r2,0,0])
            circle(r=r1);
}

//splinetest();
servo_gear();
//projection(cut = true)
//    translate([0,0,-2])
//testgear();