#pragma once

BEGIN_TEE

enum InterpolationType
{
	INTERPOLATION_NONE,
	INTERPOLATION_LINEAR,
	INTERPOLATION_HERMITE,
	INTERPOLATION_BEZIER
};

enum InterpolationType;
struct  AnimationTime
{
	AnimationTime()
	{
		start = 0;
		end = 0;
		current = 0;
		last = 0;
		weight = 1;
	}

	uint start;
	uint end;
	uint current;
	uint last;

	float weight;

	__inline void safeSetTime(uint time)
	{
		current = time;
		if (current < start)  current = start;
		if (current > end)    current = end;
	}

	__inline uint length(void) { return end - start; }

	__inline void toInterpolateMode()
	{
		current = uint(current * 0.03f);
		current++;
		current = (uint)(current * 33.33333333f);
	}
};

template<class T> struct KeyFrame
{
	KeyFrame() : time(0)
	{
		time = 0;
	}

	KeyFrame(uint time, const T& v)
	{
		this->time = time;
		this->v = v;
	}

	KeyFrame(uint time, const T& v, const T& in, const T& out)
	{
		this->time = time;
		this->v = v;
		//this->in = in;
		//this->out = out;
	}

	bool operator==(uint time)
	{
		return this->time == time;
	}

	bool operator<(const KeyFrame<T>& rhs)
	{
		return this->time < rhs.time;
	}

	uint time;
	T v;
	//T in;
	//T out;
};

template<class T>
inline bool kf_less(const KeyFrame<T>& kf1, const KeyFrame<T>& kf2)
{
	return kf1.time < kf2.time;
}

typedef std::pair<int, int>		KeyFrameRange;
typedef std::pair<uint, uint>	KeyFrameTimeRange;

template<class T> class KeyFrames
{
private:
	typedef std::vector<KeyFrame<T> >	KeyFrameList;
	typedef std::vector<KeyFrameRange>	KeyFrameRangeList;

	KeyFrameList			m_KeyFrames;
	KeyFrameRangeList		m_KeyFrameRanges;
	InterpolationType		m_InterpolationType;
	typename KeyFrameList::iterator	m_it;

	/// Static
	bool					m_bStatic;
	T						m_StaticData;

	/// GlobalSequence
	int						m_GlobalSequenceId;
	//int                     m_prevFrame;

public:
	void clear()
	{
		m_KeyFrames.clear();
		m_KeyFrameRanges.clear();
	}
	void setInterpolationType(InterpolationType interpolationType)
	{
		m_InterpolationType = interpolationType;
	}

	InterpolationType getInterpolationType()
	{
		return m_InterpolationType;
	}

	int addKeyFrameRange(const KeyFrameRange& keyFrameRange)
	{
		size_t size = m_KeyFrameRanges.size();
		m_KeyFrameRanges.push_back(keyFrameRange);
		return size;
	}

	int addKeyFrameRange(const KeyFrameTimeRange& keyFrameTimeRange)
	{
		int first = -1;
		int second = -1;

		size_t size = m_KeyFrames.size();
		for (size_t i = 0; i < size; i++)
		{
			KeyFrame<T>& keyFrame = m_KeyFrames[i];
			if (keyFrame.time == keyFrameTimeRange.first)
				first = i;
			if (keyFrame.time == keyFrameTimeRange.second)
				second = i;

			if (first != -1 && second != -1)break;
		}

		KeyFrameRange keyFrameRange;
		keyFrameRange.first = first;
		keyFrameRange.second = second;

		return addKeyFrameRange(keyFrameRange);
	}

	void setStaticData(const T& data)
	{
		m_StaticData = data;
		m_bStatic = true;
	}

	void getStaticData(T& data)
	{
		data = m_StaticData;
	}

	const T & getKeyFrameValue(size_t index){
		if (m_bStatic)
			return m_StaticData;
		return m_KeyFrames[index].v;
	}

	void addKeyFrame(const KeyFrame<T>& keyFrame)
	{
		m_KeyFrames.push_back(keyFrame);
		m_bStatic = false;
	}

	// 添加一组有序的关键帧
	void addKeyFrames(const KeyFrames<T>& keyFrames)
	{
		for (size_t i = 0; i < keyFrames.m_KeyFrames.size(); i++)
			m_KeyFrames.push_back(keyFrames.m_KeyFrames[i]);
		//m_KeyFrames.insert(m_KeyFrames.end(), keyFrames.m_KeyFrames.begin(), keyFrames.m_KeyFrames.end());
		m_bStatic = false;
	}

	// 添加一组有序的关键帧 // 本实现仅适用于vector实现的列表
	void addKeyFramesEx(const KeyFrames<T>& keyFrames)
	{
		if (keyFrames.numKeyFrames() <= 0) return;
		int size1 = keyFrames.m_KeyFrames.size();
		int size2 = m_KeyFrames.size();
		m_KeyFrames.resize(size1 + size2);
		memcpy(&m_KeyFrames[size2], &(keyFrames.m_KeyFrames[0]), sizeof(keyFrames.m_KeyFrames[0])*size1);
		m_bStatic = false;
	}

	// 插入一组有序的关键帧
	void insertKeyFrames(KeyFrames<T>& keyFrames)
	{
		if (keyFrames.numKeyFrames() <= 0) return;
		KeyFrame<T>* kf = keyFrames.getKeyFrame(0);
		KeyFrameList::iterator insert_pos = std::upper_bound(m_KeyFrames.begin(), m_KeyFrames.end(), *kf, &kf_less<T>);
		m_KeyFrames.insert(insert_pos, keyFrames.m_KeyFrames.begin(), keyFrames.m_KeyFrames.end());
		m_bStatic = false;
	}

	void reserve(size_t n)
	{
		m_KeyFrames.reserve(n);
	}

	void sort()
	{
		if (m_KeyFrames.size() > 1)
			std::sort(m_KeyFrames.begin(), m_KeyFrames.end());
	}

	size_t numKeyFrames() const
	{
		return m_KeyFrames.size();
	}

	size_t getPrevFrame(uint time){
		uint prev, next;
		getTimeFrame(time, prev, next);
		return prev;
	}
	size_t getNextFrame(uint time){
		uint prev, next;
		getTimeFrame(time, prev, next);
		return next;
	}

	void getTimeFrame(uint time, uint & prev, uint & next){
		if (m_bStatic){
			prev = next = 0;
			return;
		}
		if (time >= (*--m_KeyFrames.end()).time){
			prev = next = m_KeyFrames.size() - 1;
			return;
		}
		prev = 0;
		next = m_KeyFrames.size() - 1;
		uint mid;
		while (prev < next){
			mid = (prev + next) >> 1;
			if (time == m_KeyFrames[mid].time){
				prev = next = mid;
				return;
			}
			else if (time > m_KeyFrames[mid].time){
				prev = mid + 1;
			}
			else{
				if (mid > 0)
					next = mid - 1;
				else
					break;//不break可能会死循环
			}
		}
		if (m_KeyFrames[prev].time > time && prev > 0)
			--prev;
		if (m_KeyFrames[next].time < time && next < m_KeyFrames.size() - 1)
			++next;
		if (next >= m_KeyFrames.size())
			next = m_KeyFrames.size() - 1;
	}

	T getFrame(const AnimationTime *pTime)
	{
		if (m_bStatic)return m_StaticData;

		size_t size = m_KeyFrames.size();
		uint lastTime = m_KeyFrames[size - 1].time;
		uint time = pTime->current;
		//if(lastTime)time %= lastTime;

		//时间在第一帧之前
		if (time < m_KeyFrames[0].time)
			return m_StaticData;

		//在最后一帧之后
		if (time >= m_KeyFrames[size - 1].time)return m_KeyFrames[size - 1].v;

		KeyFrame<T> *pKeyFrame1 = 0;
		KeyFrame<T> *pKeyFrame2 = 0;

		KeyFrameRange range;
		range.first = 0;
		range.second = size - 1;

		return _getFrame(range, pTime);
	}

	T getFrame(int keyFrameRangeIndex, const AnimationTime *pTime)
	{
		if (m_bStatic)return m_StaticData;

		if (keyFrameRangeIndex == -1)return getFrame(pTime);
		KeyFrameRange range = m_KeyFrameRanges[keyFrameRangeIndex];

		return _getFrame(range, pTime);
	}

	inline T getFrame(KeyFrameRange & range, const AnimationTime * pTime){
		if (m_bStatic) return m_StaticData;

		if (pTime->current < m_KeyFrames[range.first].time)
			return m_StaticData;
		else if (pTime->current >= m_KeyFrames[range.second].time){
			//m_prevFrame = range.first;
			return m_KeyFrames[range.second].v;
		}

		return _getFrame(range, pTime);
	}

	KeyFrame<T>* getKeyFrame(size_t index)
	{
		return &m_KeyFrames[index];
	}

	void setGlobalSequenceId(int globalSequeneId)
	{
		m_GlobalSequenceId = globalSequeneId;
	}

	int getGlobalSequenceId()
	{
		return m_GlobalSequenceId;
	}

	//inline void setPrevFrame(int prevFrame){
	//    m_prevFrame = prevFrame;
	//}

	KeyFrames<T>& operator=(const KeyFrames<T>& keyFrames)
	{
		m_KeyFrames = keyFrames.m_KeyFrames;
		m_KeyFrameRanges = keyFrames.m_KeyFrameRanges;
		m_InterpolationType = keyFrames.m_InterpolationType;
		m_bStatic = keyFrames.m_bStatic;
		m_StaticData = keyFrames.m_StaticData;
		m_GlobalSequenceId = keyFrames.m_GlobalSequenceId;

		return *this;
	}

	size_t getMemorySize() const
	{
		return sizeof(*this) + m_KeyFrames.size() * sizeof(KeyFrame<T>) + m_KeyFrameRanges.size() * sizeof(KeyFrameRange);
	}

private:
	T interpolateHermite(float r, KeyFrame<T> *pKeyFrame1, KeyFrame<T> *pKeyFrame2)
	{
		//T t = rkt::interpolateHermite<T>(r,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->in,pKeyFrame1->out);
		T t = rkt::interpolateHermite<T>(r, pKeyFrame1->v, pKeyFrame2->v, pKeyFrame2->v, pKeyFrame1->v);
		return t;
	}

	T interpolateBezier(float r, KeyFrame<T> *pKeyFrame1, KeyFrame<T> *pKeyFrame2)
	{
		//T t = rkt::interpolateBezier<T>(r,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->in,pKeyFrame1->out);
		T t = rkt::interpolateBezier<T>(r, pKeyFrame1->v, pKeyFrame2->v, pKeyFrame2->v, pKeyFrame1->v);
		return t;
	}

	T _getFrame(KeyFrameRange range, const AnimationTime *pTime)
	{
		//PROMON_FUNCTION(_T("getFrame"));
		uint time = pTime->current;
		//uint maxTime = (*--m_KeyFrames.end()).time;
		//if (time > maxTime){
		//    time %=maxTime;
		//}
		KeyFrame<T> * pKeyFrames = &*m_KeyFrames.begin();
		//if (m_prevFrame >= 0 && range.second > range.first+10){
		//    if (pTime->current < m_KeyFrames[m_prevFrame].time)
		//        m_prevFrame = range.first;
		//    if (pKeyFrames[m_prevFrame].time <= pTime->current){
		//        if (pKeyFrames[m_prevFrame+1].time >= pTime->current){
		//            range.first = m_prevFrame;
		//            range.second = m_prevFrame+1;
		//        }else if (pKeyFrames[m_prevFrame+2].time >= pTime->current){
		//            range.first = m_prevFrame+1;
		//            range.second = m_prevFrame+2;
		//        }else if (pKeyFrames[m_prevFrame+3].time >= pTime->current){
		//            range.first = m_prevFrame+2;
		//            range.second = m_prevFrame+3;
		//        }
		//    }
		//}

		//PROMON_START(_T("FindFrame"));
		while (range.first < range.second - 1)
		{
			int middle = ((range.first + range.second) >> 1);
			if (time >= pKeyFrames[range.first].time && time <= pKeyFrames[middle].time)
			{
				range.second = middle;
			}
			else
			{
				range.first = middle;
			}
		}
		//PROMON_STOP();
		//if (m_prevFrame>=0){m_prevFrame = range.first;}

		KeyFrame<T> *pKeyFrame1 = &m_KeyFrames[range.first];
		KeyFrame<T> *pKeyFrame2 = &m_KeyFrames[range.second];

		if (pKeyFrame1->time < pTime->start)
		{
			if (pKeyFrame2->time > pTime->end)
			{
				return m_StaticData;
			}
			else
			{
				return pKeyFrame2->v;
			}
		}

		if (pKeyFrame2->time > pTime->end)
		{
			return pKeyFrame1->v;
		}

		if (pKeyFrame1->time == time)return pKeyFrame1->v;
		if (pKeyFrame2->time == time)return pKeyFrame2->v;

		if (m_InterpolationType == INTERPOLATION_NONE)return pKeyFrame1->v;

		float r = (time - pKeyFrame1->time) / (float)(pKeyFrame2->time - pKeyFrame1->time);

		T t;

		switch (m_InterpolationType)
		{
		case INTERPOLATION_LINEAR:
			t = interpolate(r, pKeyFrame1->v, pKeyFrame2->v);
			break;
		case INTERPOLATION_HERMITE:
			//t = interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
			t = interpolateHermite(r, pKeyFrame1, pKeyFrame2);
			break;
		case INTERPOLATION_BEZIER:
			//t = interpolate(r,pKeyFrame1->v,pKeyFrame2->v);
			t = interpolateBezier(r, pKeyFrame1, pKeyFrame2);
			break;
		}

		return t;
	}
public:
	KeyFrames() : m_InterpolationType(INTERPOLATION_NONE), m_bStatic(true), m_GlobalSequenceId(-1)/*,m_prevFrame(0)*/
	{
		//m_KeyFrames.reserve(1000);
	}
};

//template<>
//Quaternion KeyFrames<Quaternion>::interpolateHermite(float r,KeyFrame<Quaternion> *pKeyFrame1,KeyFrame<Quaternion> *pKeyFrame2)
//{
//	//Quaternion t = rkt::interpolateHermite<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->in,pKeyFrame2->v,pKeyFrame2->out);
//	Quaternion t = rkt::interpolateHermite<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->v);
//	return t;
//}

//template<>
//Quaternion KeyFrames<Quaternion>::interpolateBezier(float r,KeyFrame<Quaternion> *pKeyFrame1,KeyFrame<Quaternion> *pKeyFrame2)
//{
//	//Quaternion t = rkt::interpolateBezier<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->in,pKeyFrame2->v,pKeyFrame2->out);
//	Quaternion t = rkt::interpolateBezier<Quaternion>(r,pKeyFrame1->v,pKeyFrame1->v,pKeyFrame2->v,pKeyFrame2->v);
//	return t;
//}

END_TEE