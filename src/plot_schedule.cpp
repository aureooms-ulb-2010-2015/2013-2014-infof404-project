
#include "../h/os/plot_schedule_svg.h"

int main(int argc, char const *argv[]){

	if(argc < 2) return 1;
	
	int time_unit=15;

	uint task_id,event_id,beg,end;
	int task_nbr,lcm;
	bool over=false;
	std::cin >> task_nbr;
	std::cin >> lcm;
	os::schedule::svg_scale scale = os::schedule::svg_scale(lcm,time_unit,task_nbr);
	svg::Document doc = os::schedule::make_SVG_doc(scale, argv[1]);


	draw_axis(doc,scale); 
	draw_task_lines(doc,scale);
	draw_time_unit_lines(doc,scale);

	while(std::cin.good() and not over){

		std::cin >> event_id;
		std::cin >> task_id;
		std::cin >> beg;
		std::cin >> end;

		over = plot(doc, task_id, event_id, beg,end,scale );
	}

	doc.save();
	return 0;
}
