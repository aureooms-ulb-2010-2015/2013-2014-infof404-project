#ifndef OS_PLOT_SCHEDULE
#define OS_PLOT_SCHEDULE 


#include "svg/namespace.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


namespace os{ 
	namespace schedule{

		const uint x_offset = 20;

		typedef struct svg_scale{
				int lcm = 200;
				int time_unit = 15;
				int task_nbr = 3;
				int width, height;


				svg_scale(){
					width =25 + lcm*time_unit;
					height =25*task_nbr + 50*task_nbr+x_offset;//25 for arrows 25 for blocks, 25 between tasks , x_offset for axis border

				}

				svg_scale(int lcm, int time_unit, int task_nbr):lcm(lcm), time_unit(time_unit), task_nbr(task_nbr){
					width =25 + lcm*time_unit+x_offset;//x_offset for axis border
					height =25*task_nbr + 50*task_nbr+x_offset;//25 for arrows 25 for blocks, 25 between tasks , x_offset for axis border
				}


			} svg_scale;

		svg::Document make_SVG_doc(svg_scale scale, const std::string& filename){
				//lcm total number of time unit needed

				svg::Dimensions dimensions(scale.width , scale.height);//width,height
				svg::Document doc(filename, svg::Layout(dimensions, svg::Layout::BottomLeft));

				return doc;

			}

		void draw_time_unit_lines(svg::Document& doc, svg_scale scale){
			svg::Stroke stroke(1,svg::Color(0,0,0));
			//stroke.linecap = "round";
			stroke.dasharray = "1,3";
			for(int i = 0; i <= scale.lcm; ++i){
				doc << svg::Line(svg::Point(x_offset + i * scale.time_unit, x_offset), svg::Point(x_offset + i * scale.time_unit, scale.height-x_offset), stroke);
			}
		}

		void draw_task_lines (svg::Document& doc, svg_scale scale ){
			std::stringstream value_s;
			for (int i = 0; i < scale.task_nbr; ++i){
				//svg::Line separator_one(svg::Point(x_offset, 25*(i+1) + 50 *i), svg::Point(scale.width-x_offset, 25*(i+1) + 50 *i), svg::Stroke(1,svg::Color(0,0,0)));

				value_s.precision(0);
				value_s  << "t" <<i;
				svg::Text text(svg::Point(0, (25*(i+1)+50*i)+25 - x_offset + 3), value_s.str(), svg::Color(0,0,0));

				//svg::Line separator_two(svg::Point(x_offset, (25*(i+1)+50*i) +50), svg::Point(scale.width-x_offset, (25*(i+1)+50*i) +50), svg::Stroke(1,svg::Color(0,0,0)));
				value_s.str("");
				//doc << separator_one<< text<<separator_two;
				doc << text;
			}

		}

		void draw_axis (svg::Document& doc, svg_scale scale ){
			//svg::Line vert_axis(svg::Point(x_offset, x_offset), svg::Point(x_offset, scale.height-x_offset), svg::Stroke(1,svg::Color(0,0,0)));
			//svg::Line hor_axis(svg::Point(x_offset, x_offset), svg::Point(scale.width-x_offset, x_offset), svg::Stroke(1,svg::Color(0,0,0)));

			// svg::Polygon hor_arrow(svg::Color(0,0,0));
			// hor_arrow << svg::Point(scale.width-8, x_offset) << svg::Point(scale.width-18, 6) << svg::Point(scale.width-18, 18);



			// doc << vert_axis << hor_axis << hor_arrow ;


			//draw graduation


			for (int i = 0; i <= scale.lcm ; i+= 2 )
			{
				std::stringstream value_s;
				value_s.precision(0);
				value_s << i;
				std::string value = value_s.str();
				int offset = x_offset - 3;
				if(value.size() > 1) offset += (value.size() - 1) * (-5);
				svg::Text text(svg::Point( i*scale.time_unit+offset , 2), value_s.str(), svg::Color(0,0,0));
				doc <<  text;
			}

		}


		void draw_vert_arrow (svg::Document& doc, svg::Point head){

			svg::Point tail (head.get_x(), head.get_y()+21);

			svg::Line arrow_body(head, tail, svg::Stroke(1,svg::Color(0,0,0)));
			svg::Polygon arrow_head(svg::Color(0,0,0));
			arrow_head << svg::Point(head.get_x(), head.get_y()-4) << svg::Point(head.get_x()+3, head.get_y()+7) << svg::Point(head.get_x()-3, head.get_y()+7);
			doc << arrow_body << arrow_head;

		}

		void draw_rectangle(svg::Document& doc, svg::Point top_left_corner, svg_scale scale, uint width = 1){

			svg::Rectangle rect = svg::Rectangle(top_left_corner, width * scale.time_unit,25,  svg::Fill(svg::Color::Transparent),svg::Stroke(1, svg::Color::Black ));
		    doc<<rect;
		}
		void draw_circle(svg::Document& doc, svg::Point center){

			int circle_diameter = x_offset;
			svg::Circle circle(center, circle_diameter, svg::Fill(svg::Color::Transparent),svg::Stroke(1, svg::Color(0, 0, 0) ) );
			doc << circle;
		}

		int get_svg_task_line(uint task_id ){

			return (task_id *50 + (task_id +1 )*25 ) + 50;//1 -> 150 // 2 -> 225    25+50+25+50+25+50
					//50 pix by task // 25 pix between tasks // +50 to get from lower to higer corner
		}

		bool plot(svg::Document& doc,uint task_id, uint event_id, uint beg, uint end, svg_scale scale){

			bool over= false;

			if(event_id==0){//new job

				svg::Point head ( beg*scale.time_unit+x_offset,get_svg_task_line(task_id)-21);
				draw_vert_arrow(doc, head);

			}
			else if(event_id==1){//new dead line

				svg::Point center = svg::Point(beg*scale.time_unit+x_offset,get_svg_task_line(task_id)-50);
				draw_circle(doc, center);
			}
			else if(event_id==2){//work between
				draw_rectangle(doc, svg::Point ( beg*scale.time_unit+x_offset,get_svg_task_line(task_id)-25),scale, end-beg);
			}
			else if(event_id==3){//non scheduable
				std::cout << "system non scheduable"<<std::endl;

				over=true;
			}
			else if(event_id==4){//EOF
				over=true;
			}
			else{
				std::cout<<"unexpected event id"<<std::endl;
			}

			return over;	
		}


	}//end namespaces
}

#endif //OS_PLOT_SCHEDULE
