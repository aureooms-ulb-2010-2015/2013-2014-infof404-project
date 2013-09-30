var analyse = function(data){
	var result = {}
	for (var i = 0; i < data.length; ++i){
		result.total++;
		var current = data[i].firstElementChild;
		var sample = {};
		while(current !== null){
			sample[current.tagName]++;
			current = current.nextElementSibling;
		}

		for(var key in result){
			if(key !== 'total' && sample[key] === undefined){
				result[key].lower = 0;
			}
		}

		for(var key in sample){
			if(result[key] === undefined){
				result[key] = {};
				result[key].lower = (i == 0)? 1 : 0;
				result[key].upper = 1;
			}

			if(sample[key] > 1){
				result[key].upper = "n";
			}
		}
	}
	return result;
}

var dump = function(result){
	console.log('total : ' + result.total);
	for(var key in result){
		console.log(key + ' (' + result[key].lower + ',' + result[key].upper + ')');
	}
}