#ifndef LIB_IO_HPP
#define LIB_IO_HPP

#include <vector>
#include <map>
#include <queue>
#include <list>
#include <set>

template<typename S, typename T>
S& operator<<(S& out, const std::vector<T>& vector){
	out << "[";
	if(!vector.empty()){
		for (size_t index = 0; index < vector.size()-1; ++index){
			out << vector[index];
			out << ", ";
		}
		out << vector[vector.size()-1];
	}
	out << "]";
	return out;
}

template<typename S, typename T>
S& operator<<(S& out, const std::list<T>& list){
	out << "[";
	if(!list.empty()){
		typename std::list<T>::const_iterator it = list.cbegin();
		out << *it;
		for (++it; it != list.cend(); ++it){
			out << ", ";
			out << *it;
		}
	}
	out << "]";
	return out;
}

template<typename S, typename T>
S& operator<<(S& out, std::queue<T> queue){
	out << "[";
	if(!queue.empty()){
		while(queue.size() > 1){
			out << queue.front();
			queue.pop();
			out << ", ";
		}
		out << queue.front();
		queue.pop();
	}
	out << "]";
	return out;
}

template<typename S, typename Key, typename T>
S& operator<<(S& out, const std::map<Key,T>& map){
	out << "{";
	if (map.size() > 0){
		typename std::map<Key,T>::const_iterator it, pen = --map.end();
		for(it = map.begin(); it != pen; ++it){
			out << it->first;
			out << " : ";
			out << it->second;
			out << ", ";
		}
		out << it->first;
		out << " : ";
		out << it->second;
	}
	out << "}";
	return out;
}

template<typename S, typename Key, typename T>
S& operator<<(S& out, const std::multimap<Key,T>& map){
	out << "{";
	if (map.size() > 0){
		typename std::multimap<Key,T>::const_iterator it, pen = --map.end();
		for(it = map.begin(); it != pen; ++it){
			out << it->first;
			out << " : ";
			out << it->second;
			out << ", ";
		}
		out << it->first;
		out << " : ";
		out << it->second;
	}
	out << "}";
	return out;
}

template<typename S, typename T>
S& operator<<(S& out, const std::set<T>& set){
	out << "[";
	if(set.size() > 0){
		bool first = true;
		for(const T& val : set){
			if(!first) out << ", ";
			out << val;
			first = false;
		}
	}
	out << "]";
	return out;
}


#endif //LIB_IO_HPP
