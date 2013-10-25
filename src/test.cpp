#include <iostream>

#include "svg/namespace.hpp"

svg::Document make_SVG_doc(int lcm, int task_nbr, int size_of_time_unit ){
		//lcm total number of time unit needed

		svg::Dimensions dimensions(25 + lcm*size_of_time_unit , 100 + 50*task_nbr);//width,height
		svg::Document doc("svg/plot_res.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

		return doc;

	}

void draw_axis (svg::Document& doc, int width, int height, int time_unit, int lcm ){
	svg::Line vert_axis(svg::Point(12, 12), svg::Point(12, height-12), svg::Stroke(2,svg::Color(20, 30, 40)));
	svg::Line hor_axis(svg::Point(12, 12), svg::Point(width-12, 12), svg::Stroke(2,svg::Color(20, 30, 40)));

	svg::Polygon hor_arrow(svg::Color(20, 30, 40));
	hor_arrow << svg::Point(width-12, 12) << svg::Point(width-18, 6) << svg::Point(width-18, 18);



	doc << vert_axis << hor_axis << hor_arrow ;


	//draw graduation

	int max_grad = lcm * time_unit;

	std::cout<<max_grad<<std::endl;
	for (int i = 0; i <= lcm ; i+=20)
	{
		std::stringstream value_s;
		value_s.precision(0);
		value_s  << i;
		int offset = 9;
		if (i>9){offset=0;}
		else if (i>99){offset=-9;}
		svg::Text text(svg::Point( i*time_unit+offset , 2), value_s.str(), svg::Color(20, 30, 40));
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

void draw_rectangle(svg::Document& doc, svg::Point top_left_corner, int time_unit){
	
	svg::Rectangle rect = svg::Rectangle(top_left_corner,time_unit,25);
    doc<<rect;
}
void draw_circle(svg::Document& doc, svg::Point center){
	int circle_diameter = 12;
	svg::Circle circle(center, circle_diameter, svg::Fill(svg::Color(20, 30, 40)));
	doc << circle;
}


int main(){

	 



	 {
	 	std::cout << "SVG TEST" << std::endl;
	 	//PARAM
	 	int lcm = 1000;
	 	int task_nbr = 3;
	 	int time_unit = 10;
	 	//ATTRIBUTS
    	int width =25 + lcm*time_unit;
    	int height =100 + 50*task_nbr;//25 for arrows 25 for blocks
    	

    	svg::Document doc = make_SVG_doc(lcm,task_nbr,time_unit);
    	draw_axis(doc,width,height,time_unit,lcm ); 
    	




   		

	 	doc.save();
	 }
	return 0;
}


