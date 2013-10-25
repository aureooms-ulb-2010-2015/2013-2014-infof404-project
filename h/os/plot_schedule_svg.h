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
					height =25*task_nbr + 50*task_nbr+12;//25 for arrows 25 for blocks, 25 between tasks , 12 for axis border

				}

				svg_scale(int lcm, int time_unit, int task_nbr):lcm(lcm), time_unit(time_unit), task_nbr(task_nbr){
					width =25 + lcm*time_unit+12;//12 for axis border
					height =25*task_nbr + 50*task_nbr+12;//25 for arrows 25 for blocks, 25 between tasks , 12 for axis border
				}


			} svg_scale;

		svg::Document make_SVG_doc(svg_scale scale){
				//lcm total number of time unit needed

				svg::Dimensions dimensions(scale.width , scale.height);//width,height
				svg::Document doc("svg/plot_res.svg", svg::Layout(dimensions, svg::Layout::BottomLeft));

				return doc;

			}

		void draw_task_lines (svg::Document& doc, svg_scale scale ){
			std::stringstream value_s;
			for (int i = 0; i < scale.task_nbr; ++i)
			{
				svg::Line separator_one(svg::Point(12, 25*(i+1) + 50 *i), svg::Point(scale.width-12, 25*(i+1) + 50 *i), svg::Stroke(1,svg::Color(0,0,0)));

				value_s.precision(0);
				value_s  << "t"<<i;
				svg::Text text(svg::Point(0, (25*(i+1)+50*i) +25), value_s.str(), svg::Color(0,0,0));

				svg::Line separator_two(svg::Point(12, (25*(i+1)+50*i) +50), svg::Point(scale.width-12, (25*(i+1)+50*i) +50), svg::Stroke(1,svg::Color(0,0,0)));
				value_s.str("");
				doc << separator_one<< text<<separator_two;
			}

		}

		void draw_axis (svg::Document& doc, svg_scale scale ){
			svg::Line vert_axis(svg::Point(12, 12), svg::Point(12, scale.height-12), svg::Stroke(2,svg::Color(0,0,0)));
			svg::Line hor_axis(svg::Point(12, 12), svg::Point(scale.width-12, 12), svg::Stroke(2,svg::Color(0,0,0)));

			svg::Polygon hor_arrow(svg::Color(0,0,0));
			hor_arrow << svg::Point(scale.width-12, 12) << svg::Point(scale.width-18, 6) << svg::Point(scale.width-18, 18);



			doc << vert_axis << hor_axis << hor_arrow ;


			//draw graduation


			for (int i = 0; i <= scale.lcm ; i+= 10 )
			{
				std::stringstream value_s;
				value_s.precision(0);
				value_s  << i;
				int offset = 9;
				if (i>9){offset=0;}
				else if (i>99){offset=-9;}
				svg::Text text(svg::Point( i*scale.time_unit+offset , 2), value_s.str(), svg::Color(0,0,0));
				doc <<  text;
			}

		}


		void draw_vert_arrow (svg::Document& doc, svg::Point head){
		std::cout <<"drawing arrow on  "<<" x="<<head.get_x() << " y="<<head.get_y() <<std::endl;

			svg::Point tail (head.get_x(), head.get_y()+25);

			svg::Line arrow_body(head, tail, svg::Stroke(2,svg::Color(0,0,0)));
			svg::Polygon arrow_head(svg::Color(0,0,0));
			arrow_head << head << svg::Point(head.get_x()+3, head.get_y()+7) << svg::Point(head.get_x()-3, head.get_y()+7);
			doc << arrow_body << arrow_head;

		}

		void draw_rectangle(svg::Document& doc, svg::Point top_left_corner, svg_scale scale){
			std::cout <<"drawing Rectangle on  "<<" x="<<top_left_corner.get_x() << " y="<<top_left_corner.get_y() <<std::endl;

			svg::Rectangle rect = svg::Rectangle(top_left_corner, scale.time_unit,25,  svg::Fill(svg::Color(0,0,0)),svg::Stroke(1, svg::Color::White ));
		    doc<<rect;
		}
		void draw_circle(svg::Document& doc, svg::Point center){
			std::cout <<"drawing circle on  "<<" x="<<center.get_x() << " y="<<center.get_y() <<std::endl;

			int circle_diameter = 12;
			svg::Circle circle(center, circle_diameter, svg::Fill(svg::Color::Transparent),svg::Stroke(1, svg::Color(0, 0, 0) ) );
			doc << circle;
		}

		int get_svg_task_line(uint task_id ){

			return (task_id *50 + (task_id +1 )*25 ) + 50;//1 -> 150 // 2 -> 225    25+50+25+50+25+50
					//50 pix by task // 25 pix between tasks // +50 to get from lower to higer corner
		}

		bool plot(svg::Document& doc,uint task_id, uint event_id, uint beg, uint end, svg_scale scale){
			std::cout <<"Tid "<<task_id << " event "<<event_id << " beg "<<beg<<" end "<<end<<std::endl;

			bool over= false;

			if(event_id==0){//new job

				std::cout << "new job"<<std::endl;
				svg::Point head ( beg*scale.time_unit+12,get_svg_task_line(task_id)-25);
				draw_vert_arrow(doc, head);

			}
			else if(event_id==1){//new dead line
				std::cout << "new dead line"<<std::endl;

				svg::Point  ( beg*scale.time_unit+12,get_svg_task_line(task_id)-50);
				draw_circle(doc,svg::Point ( (beg*scale.time_unit) + 12 ,get_svg_task_line(task_id)-12 ) );
			}
			else if(event_id==2){//work between
				std::cout << "new work"<<std::endl;

				while(beg<=end){
					draw_rectangle(doc, svg::Point ( beg*scale.time_unit+12,get_svg_task_line(task_id)-25),scale);
					beg+=1;
				}

			}
			else if(event_id==3){//non scheduable
				std::cout << "non scheduable"<<std::endl;

				over=true;
			}
			else if(event_id==4){//EOF
				std::cout << "OVER"<<std::endl;
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
