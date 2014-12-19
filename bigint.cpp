#include "bigint.h"

//Constructor
bigint::bigint() {
	positive = true;
	number.reserve(1024);
}
//Adding
bigint bigint::operator+(bigint const &b) {
	bigint c = *this;
	return c+=b;
}
bigint& bigint::operator+=(bigint const &b) {
	if(b.positive == false) {
		*this -= b;
		return *this;
	}
	vector<int>::iterator
		it1 = number.begin();
	vector<int>::const_iterator
		it2 = b.number.begin();
	int sum = 0;
	while(it1 != number.end() || it2 != b.number.end()) {
		if(it1 != number.end()) {
			sum += *it1;
		} else {
			number.push_back(0);
		}
		if(it2 != b.number.end()) {
			sum += *it2;
			it2++;
		}
		*it1 = sum%base;
		it1++;
		sum /= base;
	}
	if(sum) number.push_back(1);
	return *this;
}
bigint& bigint::operator+=(long long b) {
	vector<int>::iterator it = number.begin();
	if(skip > number.size()) {
		number.insert(number.end(), skip-number.size(), 0);
	}
	it += skip;
	while(b) {
		if(it != number.end()) {
			*it += b%base;
			b /= base;
			b += *it/base;
			*it %= base;
			++it;
		} else {
			number.push_back(0);
			it = number.end()-1;
		}
	}
	return *this;
}

//Subtraction
bigint bigint::operator-(bigint const &b) {
	bigint c = *this;
	return c-=b;
}
bigint& bigint::operator-=(bigint const &b) {
	vector<int>::iterator
		it1 = number.begin();
	vector<int>::const_iterator
		it2 = b.number.begin();
	int dif = 0;
	while(it1 != number.end() || it2 != b.number.end()) {
		if(it1 != number.end()) {
			dif += *it1;
			it1++;
		}
		if(it2 != b.number.end()) {
			dif -= *it2;
			it2++;
		}
		if(dif < 0) {
			*(it1-1) = (dif*(-1))%base;
			dif = -1;
		} else {
			*(it1-1) = dif%base;
			dif /= base;
		}
	}
	if(dif < 0) positive = false;
	return *this;
}

//Multiplication
bigint bigint::operator*(bigint const &b) {
	if(b.number.size() == 1) return *this *= b.number[0];
	vector<int>::iterator it1;
	vector<int>::const_iterator it2;
	bigint c;
	for(it1 = number.begin(); it1 != number.end(); ++it1) {
		for(it2 = b.number.begin(); it2 != b.number.end(); ++it2) {
			c.skip = it1-number.begin() + it2-b.number.begin();
			c += (long long)(*it1)*(*it2);
		}
	}
	c.skip = 0;
	return c;
}
bigint& bigint::operator*=(bigint const &b) {
	*this = *this * b;
	return *this;
}
bigint bigint::operator*(long long const &b) {
	bigint c = *this;
	return c *= b;
}
bigint& bigint::operator*=(int const &b) {
	vector<int>::iterator it = number.begin();
	long long sum = 0;
	while(it != number.end()) {
		sum += (long long)(*it)*b;
		*it = sum%base;
		sum /= base;
		++it;
	}
	if(sum) number.push_back(sum);
	return *this;
}

//Power
bigint bigint::pow(int const &power, map<int, bigint> &lookup) {
	if(power == 1) 			return *this;
	if(lookup.count(power)) return lookup[power];

	int closestPower = 1;
	while(closestPower < power) closestPower <<= 1;
	closestPower >>= 1;

	if(power == closestPower) lookup[power] = pow(power/2, lookup)*pow(power/2, lookup);
	else lookup[power] = pow(closestPower, lookup)*pow(power-closestPower, lookup);

	return lookup[power];
}
bigint& bigint::pow(int const &power) {
	map<int, bigint> lookup;
	*this = pow(power, lookup);
	return *this;
}

//Mod
bigint bigint::operator%(bigint const &b) {
	bigint c;
	if(*this < b) {
		return c;
	}
	if(*this == b) {
		c = 1;
		return c;
	}
	vector<int>::reverse_iterator it = number.rbegin();
	vector<int>::const_reverse_iterator it2 = b.number.rbegin();
	bigint t;
//	t.number.assign()
	return *this;
}

//Compare
bool bigint::operator<(bigint const &a) const {
	if(positive && !a.positive) return false;
	if(!positive && a.positive) return true;
	bool value = true;
	if(!positive && !a.positive) value = false;
	if(number.size() < a.number.size()) return value;
	if(number.size() > a.number.size()) return !value;
	for(int i(0); i < number.size(); i++) {
		if(number[i] < a.number[i]) return value;
		if(number[i] > a.number[i]) return !value;
	}
	return false; // ==
}
bool bigint::operator==(bigint const &a) const {
	if(positive != a.positive) return false;
	if(number.size() != a.number.size()) return false;
	for(int i(0); i < number.size(); i++) {
		if(number[i] != a.number[i]) return false;
	}
	return true;
}

//Allocation
bigint bigint::operator=(const long long &a) {
	number.clear();
	long long t = a;
	do {
		number.push_back(t%base);
		t /= base;
	} while(t != 0);
	return *this;
}

//Access
int bigint::operator[](int const &b) {
	return toString()[b]-'0';
}

//Trivia
int bigint::digits() {
	int segments = number.size();
	if(segments == 0) return 0;
	int digits = 9*(segments-1);
	digits += segmentLength(number.back());
	return digits;
}
int bigint::trailingZeros() {
	if(number.empty() || (number.size() == 1 && number[0] == 0)) return 1;
	int zeros = 0;
	vector<int>::const_iterator it = number.begin();
	if(number.size() > 1) {
		for(; it != number.end()-1 && *it == 0; ++it) {
			zeros += 9;
		}
	}
	int a = *it;
	while(a%10 == 0 && a) {
		zeros++;
		a /= 10;
	}
	return zeros;
}

//Helpers
void bigint::clear() {
	number.clear();
	positive = true;
	skip = 0;
}
string bigint::toString() {
	stringstream stream;
	while(number.size() && number.back() == 0) number.pop_back();
	if(!number.size()) return "0";
	if(!positive) stream << '-';
	vector<int>::const_reverse_iterator it = number.rbegin();
	stream << *it;
	if(it != number.rend()) ++it;
	for(;it != number.rend(); ++it) {
		for(int i(0), len = segmentLength(*it); i < 9-len; ++i) stream << '0';
		if(*it) stream << *it;
	}
	return stream.str();
}

//Input&Output
ostream &operator<<(ostream &out, bigint a) {
	out << a.toString();
	return out;
}
istream &operator>>(istream &in, bigint &a) {
	string str;
	in >> str;
	int size = str.length();
	if(str[0] == '-') a.positive = false;
	while(true) {
		if(size <= 0) break;
		if(!a.positive && size <= 1) break;

		int length = 0;
		int num = 0;
		int prefix = 1;
		for(int i(size-1); i >= 0 && i >= (int)size-9; i--) {
			if(str[i] < '0' || str[i] > '9') break;
			num += (str[i]-'0')*prefix;
			prefix *= 10;
			length++;
		}
		a.number.push_back(num);
		size -= length;
	}
	return in;
}
int bigint::segmentLength(int segment) {
	int length = 0;
	while(segment) {
		segment /= 10;
		length++;
	}
	return length;
}
