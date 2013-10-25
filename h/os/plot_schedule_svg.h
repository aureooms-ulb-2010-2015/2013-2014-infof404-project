#ifndef OS_PLOT_SCHEDULE
#define OS_PLOT_SCHEDULE 


#include "svg/namespace.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace os{ 
namespace schedule{

typedef struct svg_scale{
		int lcm = 200;
		int time_unit = 15;
		int task_nbr = 3;
		int width, height;


		svg_scale(){
			width =25 + lcm*time_unit;
			height =100 + 50*task_nbr;//25 for arrows 25 for blocks

		}

		svg_scale(int lcm, int time_unit, int task_nbr):lcm(lcm), time_unit(time_unit), task_nbr(task_nbr){
			width =25 + lcm*time_unit+12;//12 for axis border
			height =25*task_nbr + 50*task_nbr+12;//25 for arrows 25 for blocks, 25 between tasks , 12 for axis border
		}


	} svg_scale;

svg::Document make_SVG_doc(svg_scale scale){
		//lcm total number of time unit needed

		svg::Dimensions dimensions(25 + scale.lcm * scale.time_unit , 100 + 50 * scale.task_nbr);//width,height
		svg::Document doc("svg/plot_res.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

		return doc;

	}

void draw_axis (svg::Document& doc, svg_scale scale ){
	svg::Line vert_axis(svg::Point(12, 12), svg::Point(12, scale.height-12), svg::Stroke(2,svg::Color(20, 30, 40)));
	svg::Line hor_axis(svg::Point(12, 12), svg::Point(scale.width-12, 12), svg::Stroke(2,svg::Color(20, 30, 40)));

	svg::Polygon hor_arrow(svg::Color(20, 30, 40));
	hor_arrow << svg::Point(scale.width-12, 12) << svg::Point(scale.width-18, 6) << svg::Point(scale.width-18, 18);



	doc << vert_axis << hor_axis << hor_arrow ;


	//draw graduation

	int max_grad = lscale.cm * scale.time_unit;

	for (int i = 0; i <= lcm ; i+=20)
	{
		std::stringstream value_s;
		value_s.precision(0);
		value_s  << i;
		int offset = 9;
		if (i>9){offset=0;}
		else if (i>99){offset=-9;}
		svg::Text text(svg::Point( i*scale.time_unit+offset , 2), value_s.str(), svg::Color(20, 30, 40));
		doc <<  text;
	}

}


void draw_vert_arrow (svg::Document& doc, svg::Point head){

	svg::Point tail (head.get_x(), head.get_y()+25);

	svg::Line arrow_body(head, tail, svg::Stroke(2,svg::Color(20, 30, 40)));
	svg::Polygon arrow_head(svg::Color(20, 30, 40));
	arrow_head << head << svg::Point(head.get_x()+3, head.get_y()+7) << svg::Point(head.get_x()-3, head.get_y()+7);
	doc << arrow_body << arrow_head;

}

void draw_rectangle(svg::Document& doc, svg::Point top_left_corner, svg_scale scale){
	
	svg::Rectangle rect = svg::Rectangle(top_left_corner, scale.time_unit,25);
    doc<<rect;
}
void draw_circle(svg::Document& doc, svg::Point center){
	int circle_diameter = 12;
	svg::Circle circle(center, circle_diameter, svg::Fill(svg::Color(20, 30, 40)));
	doc << circle;
}

int get_svg_task_line(svg_scale scale, uint task_id ){

	return scale.get
}

bool plot(svg::Document doc,uint task_id, uint event_id, uint beg, uint end, svg_scale scale){

	bool over= false;

	if(event_id==0){//new job

		//draw_vert_arrow(doc,)

	}
	else if(event_id==1){//new dead line

	}
	else if(event_id==2){//work between

	}
	else if(event_id==3){//non scheduable
		over=true;
	}
	else if(event_id==4){//EOF
		over=true;
	}

		
}

int main (){
 	
 	

 	int time_unit=15;


	svg_scale scale;
	svg::Document doc = make_SVG_doc(scale);
	draw_axis(doc,scale); 
    

	
	uint task_id,event_id,beg,end;
	bool over=false,got_scale_values = false;

	std::ifstream ifstream;
	ifstream.open(file_name);
	while(std::cin.good() and not over){

		if (not got_scale_values)
		{
			std::cin >> task_nbr;
			std::cin >> lcm;
			scale=svg_scale(lcm,time_unit,task_nbr);
			got_scale_values= true;
		}

		std::cin >> task_id;
		std::cin >> event_id;
		std::cin >> beg;
		std::cin >> end;

		over = plot(doc, task_id, event_id, beg,end,scale );
	}

	ifstream.close();
	doc.save();
	return 0;
	}
}//end namespaces
}

#endif //OS_PLOT_SCHEDULE
