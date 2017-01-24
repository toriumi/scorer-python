
#include <algorithm>

template <typename T>
struct ARG_SORTER
{
	T	val;
	int idx;

	bool operator<( const struct ARG_SORTER& right ) const{
		return val < right.val;
	}
};

template <typename T>
void argsort_vector(const std::vector<T>& vec, std::vector<int>& idx)
{
	int vec_size = vec.size();
	std::vector<struct ARG_SORTER<T>> sort_pairs;
	for(int i=0; i<vec_size; i++){
		struct ARG_SORTER<T> argsorter;
		argsorter.val = vec[i];
		argsorter.idx = i;
		sort_pairs.push_back(argsorter);
	}

	std::sort(sort_pairs.begin(), sort_pairs.end()); 
	
	idx.clear();
	for(int i=0; i<vec_size; i++){
		idx.push_back(sort_pairs[i].idx);
	}
}

template<typename T> 
int max_arg(const std::vector<T>& vec, T& max_val)
{
	if(vec.empty())
		return -1;
	int num = vec.size();
	int max_i = 0;
	max_val = vec[0];
	for(int i=1; i<num; i++){
		if(max_val < vec[i]){
			max_val = vec[i];
			max_i = i;
		}
	}
	return max_i;
}


template<typename T> 
int min_arg(const std::vector<T>& vec, T& min_val)
{
	if(vec.empty())
		return -1;
	int num = vec.size();
	int min_i = 0;
	min_val = vec[0];
	for(int i=1; i<num; i++){
		if(min_val > vec[i]){
			min_val = vec[i];
			min_i = i;
		}
	}
	return min_i;
}

