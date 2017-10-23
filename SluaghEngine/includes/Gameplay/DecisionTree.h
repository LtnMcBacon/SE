#pragma once
#include <vector>
#include <map>
#include <iostream>

#undef max
#undef min

// http://people.cs.pitt.edu/~milos/courses/cs2750-Spring2014/Lectures/Class13.pdf
// https://en.wikipedia.org/wiki/ID3_algorithm
// https://en.wikipedia.org/wiki/Decision_tree_learning

/**
*
* @brief The interface class used by the Decision Tree
*
* @details For each feature that are to be added to the Decision Tree, a class with this interface must be
* created as a wrapper. All functions must be implemented for the Decision Tree to work properly.
* 
* @warning Make sure that the functions are properly implemented! 
*
* @sa DecisionTree
*
**/
class IFeature
{
private:

public:
	
	/**
	 * @brief -1 indicates that the feature is continous, and splits should be decided by the Decision Tree
	 */
	virtual int SplitingPoints() = 0;
	
	/**
	 *  @brief Return in what "group" the sample should be placed in during a split. 
	 *  For an enum, this is simply returning int(ENUM_VALUE)
	 */
	virtual int SplitingGroupForSample() = 0;
	
	/**
	 *  @brief Convert the feature into a float, for splitting in a continous space.
	 */
	virtual float ConvertToFloat() = 0;

	/**
	 * @brief Copy the IFeature (return pointer to sub-class, copy data)
	 */
	virtual IFeature* copy() = 0;

	IFeature()
	{
	};

	~IFeature()
	{
	};
};

/**
*
* @brief The DecisionTree class used by the Sluagh
*
* @details The DecisionTree takes in a labeled FeatureList and build a tree out of it.
* Each Feature must be implemented with the IFeature interface
*
* @warning Not tested in a "real" environment! 
*
* @sa IFeature, FeatureList
*
**/
template <typename Answer>
class DecisionTree
{
public:
	typedef Answer value_type;
	/**
	*
	* @brief The DecisionTree class used by the Sluagh
	*
	* @details The DecisionTree takes in a labeled FeatureList and build a tree out of it.
	* Each Feature must be implemented with the IFeature interface.
	* 
	* @warning Not tested in a "real" environment!
	* @warning The data type representing the Answer must have operator== implemented!
	* @warning The data type representing the Answer must be castable to int!
	*
	* @sa IFeature, FeatureList
	*
	**/
	struct FeatureList
	{
		std::vector<IFeature*> features;
		Answer result;

		FeatureList() = delete;

		FeatureList(std::vector<IFeature*> &init, Answer result) :
			result(result)
		{
			for (auto feature : init)
				features.push_back(feature->copy());
		}

		~FeatureList()
		{
			for (auto feature : features)
				delete feature;
		}

		inline bool Compare(Answer compareToo)
		{
			return result == compareToo;
		}

		inline int ConvertAnswerToInt() const
		{
			return int(result);
		}
	};

	/**
	* @brief	Create the tree and build it.
	*
	* @details	With the data sent in through the "StartData", train the FeatureTree to make decisions
	*
	* @param[in] startData A vector of FeatureLists that the 
	* @param[in] numberOfPossibleAnswers Parameter description
	*
	* @warning MUST SEND IN FEATURES!
	*
	*/
	DecisionTree(std::vector<FeatureList*>& startData, int numberOfPossibleAnswers);
	DecisionTree()
	{
		numberOfPossibleAnswers = 0;
	}

	void Train(std::vector<FeatureList*>& startData, int numberOfPossibleAnswers);

	~DecisionTree();

	/**
	 *
	 * @brief Decide the action to take from given features.
	 * 
	 */
	Answer Decide(std::vector<IFeature*> &toClassify)
	{
		return root->returnAnswer(toClassify, this);
	}

protected:
	std::vector<int> splitingGroups;
	friend class Node;

	class Node
	{
	public:
		Node()
		{
			featureForSplit = -1;
			splitData = -1;
			maxLimit = 0.f;
			minLimit = 0.f;
			numberOfChildren = 0;
			children = nullptr;
		};

		~Node()
		{
			for (int i = 0; i < numberOfChildren; i++)
			{
				delete children[i];
			}
			if (numberOfChildren)
				delete[] children;
		}

		Answer answer;
		int featureForSplit = -1;
		int splitData = -1;
		float maxLimit = 0.f;
		float minLimit = 0.f;
		int numberOfChildren = 0;
		Node** children = nullptr;

		/**
		 * @brief Return the answer from the features.
		 */
		Answer returnAnswer(std::vector<IFeature*> &toClassify, DecisionTree* owner)
		{
			/*Check if our feature can be used to split on. REFACTOR THIS!*/
			if (owner->splitingGroups[featureForSplit] != -1)
				if (splitData == -1)
					return answer;
				else
					return children[toClassify[splitData]->SplitingGroupForSample()]->returnAnswer(toClassify, owner);
			else
			{
				if (!numberOfChildren) /*We have no children, thus this must be the answer*/
					return answer;
				float minValue = std::numeric_limits<float>::max();
				int minChild = -1;
				float maxValue = -std::numeric_limits<float>::max();
				int maxChild = -1;
				float value = toClassify[splitData]->ConvertToFloat();
				for (int i = 0; i < numberOfChildren; i++)
				{
					if (children[i]->minLimit <= value && value <= children[i]->maxLimit)
						return children[i]->returnAnswer(toClassify, owner);
					else
					{
						if (children[i]->minLimit < minValue)
						{
							minValue = children[i]->minLimit;
							minChild = i;
						}
						if (maxValue < children[i]->maxLimit)
						{
							maxValue = children[i]->maxLimit;
							maxChild = i;
						}
					}
				}
				if (value < minValue)
					return children[minChild]->returnAnswer(toClassify, owner);
				if (maxValue < value)
					return children[maxChild]->returnAnswer(toClassify, owner);
			}
			return static_cast<Answer>(0);
		};

		/**
		 * @brief Correct the range of all nodes so that the continous space is covered.
		 */
		void CorrectRange(DecisionTree* owner)
		{
			if (owner->splitingGroups[featureForSplit] == -1)
			{
				if (numberOfChildren)
				{
					/*Sort the childs*/
					Node* temp;
					for (int i = 0; i < numberOfChildren; i++)
					{
						for (int j = i; j > 0; j--)
						{
							if (children[j]->minLimit < children[j - 1]->minLimit)
							{
								temp = children[j];
								children[j] = children[j - 1];
								children[j - 1] = temp;
							}
						}
					}

					/*Make the childs correct themselves*/
					for (int i = 0; i < numberOfChildren; i++)
						children[i]->CorrectRange(owner);

					/*Correct my childs*/

					/*Naive solution: add half the difference between the limits to
					 * previous childs maxLimit, and subtract half from this childs
					 * minLimit.
					 */
					for (int i = 1; i < numberOfChildren; i++)
					{
						float difference = (children[i-1]->maxLimit - children[i]->minLimit)*0.5f;
						children[i]->minLimit += difference;
						children[i - 1]->maxLimit -= difference;
					}
				}
			}
		}
	};

	struct VectorWrapper
	{
		std::vector<FeatureList*> list;
	};

	int numberOfPossibleAnswers;

	/**
	 * @brief Calculate the possible smallest remainder for a data set after a split.
	 */
	double Remainder(VectorWrapper* data, int sizeBeforeSplit);

	/**
	 * @brief Calculate the Entropy given number of positive and negative examples in a dataset.
	 */
	double Entropy(int positive, int negative);

	/**
	 * @brief Calculate the maximum information gain from a vector.
	 */
	double InformationGainFromVector(VectorWrapper* data);

	/**
	 * @brief Split up the data in the "data" parameter into multiple vectors, as decided by the feature that should be split on.
	 */
	void DataAfterSplit(VectorWrapper* data, int attribute, std::vector<VectorWrapper*>& returnVector);

	/**
	 * @brief Helper function to split up a continous feature into multiple vectors.
	 */
	void SplitContiniousFeature(VectorWrapper* data, int attribute, float lowValue, float highValue,
					  std::vector<VectorWrapper*>& returnVector);

	/**
	 * @brief Calculate the feature that would yield the highest information gain if a split would be done on it.
	 */
	int Gains(VectorWrapper* data, const std::vector<bool>& leftForSplit);

	/**
	 * @brief Build a (sub)tree from given data.
	 */
	Node* BuildRoot(VectorWrapper* data, std::vector<bool> leftForSplit);

	/**
	 * @brief Helper function to create a tree based on a Non-Continous feature.
	 */
	Node* BuildRootForNonContinousFeature(VectorWrapper* data, std::vector<bool> leftForSplit, int attributeToSplitOn);

	/**
	 * @brief Helper function to create a tree based on a Continous feature.
	 */
	Node* BuildRootForContiniousFeature(VectorWrapper* data, std::vector<bool> leftForSplit, int attributeToSplitOn);

	Node* root;
};

template <typename Answer>
DecisionTree<Answer>::DecisionTree(std::vector<FeatureList*>& startData, int numberOfPossibleAnswers)
{
	Train(startData, numberOfPossibleAnswers);
}

template <typename Answer>
void DecisionTree<Answer>::Train(std::vector<FeatureList*>& startData, int numberOfPossibleAnswers)
{
	this->numberOfPossibleAnswers = numberOfPossibleAnswers;
	/*Create a vector that contains information on whether a specific attribute can be split on or not.
	* Right now, this vector will be copied for each call to "BuildRoot" and "BuildRootForFloat", since each
	* subtree can split on data regardless of another subtree's split.
	*/
	std::vector<bool> leftForSplit(startData[0]->features.size(), true);
	for (auto feature : startData[0]->features)
	{
		splitingGroups.push_back(feature->SplitingPoints());
	}
	/*Create a wrapper for the vector and put the start-data in there. Needed for "Buildroot"*/
	VectorWrapper* temp = new VectorWrapper;
	temp->list = startData;

	/*This will be the root of the decision tree.*/
	this->root = BuildRoot(temp, leftForSplit);

	/*Make sure that eventual floating ranges are fixed*/
	this->root->CorrectRange(this);

	/*Done.*/
	delete temp;
}

template <typename Answer>
DecisionTree<Answer>::~DecisionTree()
{
	if (root)
		delete root;
}

template <typename Answer>
double DecisionTree<Answer>::Remainder(VectorWrapper* data, int sizeBeforeSplit)
{
	/*
	 *The smallest remainder that this split can cause.
	 */
	double minRemainder = std::numeric_limits<double>::max();

	/*
	 * Go through all possible answers for the tree, checking which answer would yield the lowest
	 * remainder.
	 */
	double divider = 1.0 / double(sizeBeforeSplit);
	for (int i = 0; i < numberOfPossibleAnswers; i++)
	{
		int positive = 0;
		int negative = 0;

		/*
		 * Convert the answer of a sample to an int and check it against i.
		 * If they are the same, this sample is positive for this possible answer.
		 * Otherwise, it's wrong and added to the negative
		 */
		for (auto& sample : data->list)
		{
			if (sample->ConvertAnswerToInt() == i)
				positive++;
			else
				negative++;
		}

		/*
		 * If there are no negative answers for this possibility, then we are done here.
		 * The minRemainder is 0.0, and thus we return this value.
		 */
		if (!negative) //Perfect split
			return 0.0;
		/*
		 *Calculate the remainder for this possible answer
		 */
		
		double remainder = (double(positive + negative) * divider) * Entropy(positive, negative);
		
		/*
		 *If this remainder is the smallest, save it down for later use.
		 */
		if (remainder <= minRemainder)
			minRemainder = remainder;
	}
	return minRemainder;
}

template <typename Answer>
double DecisionTree<Answer>::Entropy(int positive, int negative)
{
	int size = positive + negative;

	double probabilityTrue = double(positive) / double(size);
	double probabilityFalse = 1.0 - probabilityTrue;
	double entropy = 0.0;

	/*Entropy = -p(True)*log2(p(True)) - p(False)*log2(p(False))*/
	if (positive)
		entropy -= probabilityTrue * log2(probabilityTrue);
	if (negative)
		entropy -= probabilityFalse * log2(probabilityFalse);

	return entropy;
}

template <typename Answer>
double DecisionTree<Answer>::InformationGainFromVector(VectorWrapper* data)
{
	double maxGainFromSplit = 0.0;

	/*Go through all possible answers for the tree, checking which answer would yield the highest
	* gain.
	*/
	for (int i = 0; i < numberOfPossibleAnswers; i++)
	{
		int positive = 0;
		int negative = 0;

		/*Convert the answer of a sample to an int and check it against i.
		* If they are the same, this sample is positive for this possible answer.
		* Otherwise, it's wrong and added to the negative
		*/
		for (auto& sample : data->list)
			if (sample->ConvertAnswerToInt() == i)
				positive++;
			else
				negative++;

		/*Calculate the entropy for this possible answer*/
		double entropy = Entropy(positive, negative);

		/*If it is higher than the current maximum gain from a split, pick it.*/
		if (entropy > maxGainFromSplit)
			maxGainFromSplit = entropy;
	}
	return maxGainFromSplit;
}

template <typename Answer>
void DecisionTree<Answer>::DataAfterSplit(
	VectorWrapper* data, int attribute, std::vector<VectorWrapper*>& returnVector)
{
	/*
	 * If the number of spliting points are knonw, split. Otherwise,
	 * calculate them (continous, for floats).
	 */
	if (splitingGroups[attribute] != -1)
	{
		/*Create a VectorWrapper for each spliting point in a feature*/
		for (int i = 0; i < splitingGroups[attribute]; i++)
		{
			returnVector.push_back(new VectorWrapper);
		}

		/*Go through the list and put each sample inside the list their split points them too.*/
		for (auto& sample : data->list)
		{
			int vectorIndex = sample->features[attribute]->SplitingGroupForSample();
			returnVector[vectorIndex]->list.push_back(sample);
		}
	}
	else
	{
		/*If the feature is continous, we must calculate the splitting points.*/
		float max = -std::numeric_limits<float>::max();
		float min = std::numeric_limits<float>::max();

		/*Find the range of the continous feature.*/
		for (auto& sample : data->list)
		{
			float value = sample->features[attribute]->ConvertToFloat();
			if (value < min)
				min = value;
			if (max < value)
				max = value;
		}
		/*Split the continous data*/
		SplitContiniousFeature(data, attribute, min, max, returnVector);
	}
}


template <typename Answer>
void DecisionTree<Answer>::SplitContiniousFeature(
	VectorWrapper* data, int attribute, float lowValue, float highValue,
	std::vector<VectorWrapper*>& returnVector)
{
	/*Create two new vector wrappers*/
	VectorWrapper* below = new VectorWrapper;
	VectorWrapper* above = new VectorWrapper;
	below->list.reserve(returnVector.size());
	above->list.reserve(returnVector.size());

	/*Calculate the median for a split.*/
	float median = (lowValue + highValue) / 2.0f;

	/*Add the samples to the vector they fit in.*/
	for (auto sample : data->list)
	{
		if (sample->features[attribute]->ConvertToFloat() <= median)
			below->list.push_back(sample);
		else
			above->list.push_back(sample);
	}

	/*-----------------Fix the "below" list---------------------*/
	/*If the list has sample in it, we work with it.*/
	if (below->list.size())
	{
		/*If the list contains less than 50 elements, we don't split any further.*/
		if (below->list.size() <= 50)
		{
			returnVector.push_back(below);
		}
		else
		{
			/*See if all samples contains the same answer*/
			bool allEqual = true;
			Answer latestSampleResult = below->list[0]->result;
			for (auto& sample : below->list)
			{
				if (!sample->Compare(latestSampleResult))
				{
					allEqual = false;
					break;
				}
			}
			/*
			 * If all are equal, we are done with the vector. 
			 * We have an answer that we are satisfied with (100% of one possible answer).
			 */
			if (allEqual)
			{
				returnVector.push_back(below);
			}
			else
			{
				/*
				 * If there are more than 50 samples and they are not equal,
				 * the vector is recursively split.
				 */

				SplitContiniousFeature(below, attribute, lowValue, median, returnVector);
				delete below;
			}
		}
	}
	/*-----------------Fix the "above" list---------------------*/
	/*If the list has sample in it, we work with it.*/
	if (above->list.size())
	{
		/*If there are less than 50 elements in the vector, it's considered done.*/
		if (above->list.size() <= 50)
		{
			returnVector.push_back(above);
		}
		else
		{
			/*Check to see if all samples are equal*/
			bool allEqual = true;
			Answer latestSampleResult = above->list[0]->result;
			for (auto& sample : above->list)
			{
				if (!sample->Compare(latestSampleResult))
				{
					allEqual = false;
					break;
				}
			}
			/*If all samples are equal, the vector is considered done*/
			if (allEqual)
			{
				returnVector.push_back(above);
			}
			else
			{
				/*If not, we recursiverly split the new vector.*/
				SplitContiniousFeature(above, attribute, median, highValue, returnVector);
				delete above;
			}
		}
	}


}

template <typename Answer>
int DecisionTree<Answer>::Gains(VectorWrapper* data, const std::vector<bool>& leftForSplit)
{
	double gains = 0.0;
	int attributeToSplitOn = -1;
	double gainFromAttribute = 0.0;
	int size = data->list.size();

	/*Calculate the current information for the data*/
	double currentInformation = InformationGainFromVector(data);

	int counter = 0;

	/*Loop thorugh each feature that is left for spliting.*/
	for (const auto& attribute : leftForSplit)
	{
		/*
		 * If the attribute is available to be split on,
		 * the gain from splitting on this attribute is calculated
		 */
		if (attribute)
		{
			std::vector<VectorWrapper*> dataAfterSplit;
			/*Create a split on the data as if we split on this attribute*/
			DataAfterSplit(data, counter, dataAfterSplit);

			/*Calculate the remainder for this dataset*/
			double remainder = 0.0;
			for (auto& dataSet : dataAfterSplit)
			{
				remainder += Remainder(dataSet, size);
				delete dataSet;
			}

			/*Calculate the gain from the split*/
			double gainForSplit = currentInformation - remainder;

			/*If this attribute has the biggest gain, it is saved.*/
			if (gains <= gainForSplit)
			{
				gains = gainForSplit;
				attributeToSplitOn = counter;
			}
		}
		counter++;
	}
	return attributeToSplitOn;
}

template <typename Answer>
typename DecisionTree<Answer>::Node* DecisionTree<Answer>::BuildRoot(VectorWrapper* data,
																	 std::vector<bool> leftForSplit)
{
	Node* node = nullptr;
	/*Check what attribute that should be used for this split*/
	int attributeToSplitOn = Gains(data, leftForSplit);
	/*If the attribute is not continous, build the root with the BuildRootForNonContionus function*/
	if (data->list[0]->features[attributeToSplitOn]->SplitingPoints() != -1)
	{
		leftForSplit[attributeToSplitOn] = false;
		node = BuildRootForNonContinousFeature(data, leftForSplit, attributeToSplitOn);
	}
		/*Otherwise, build the root with the BuildRootForFloat function*/
	else
	{
		leftForSplit[attributeToSplitOn] = false;
		node = BuildRootForContiniousFeature(data, leftForSplit, attributeToSplitOn);
	}
	node->splitData = attributeToSplitOn;
	return node;
}


template <typename Answer>
typename DecisionTree<Answer>::Node* DecisionTree<Answer>::BuildRootForNonContinousFeature(VectorWrapper* data,
	std::vector<bool> leftForSplit,
	int attributeToSplitOn)
{
	Node* node = new Node();

	/*Save the attribute as the one this node split on.*/
	node->featureForSplit = attributeToSplitOn;

	/*Check if all samples got the same answer*/
	bool allEqual = true;
	Answer latestSampleResult = data->list[0]->result;
	for (auto& sample : data->list)
	{
		if (!sample->Compare(latestSampleResult))
		{
			allEqual = false;
			break;
		}
	}

	/*If so, we are done*/
	if (allEqual)
	{
		node->splitData = -1;
		node->answer = latestSampleResult;
		return node;
	}

	bool attributeLeft = false;

	/*Check if there is any attribute left to split on.*/
	int attributeID = 0;
	for (auto& attributeToTest : leftForSplit)
	{
		if (attributeToTest == true)
		{
			attributeLeft = true;
			break;
		}
	}

	/*Find the answer that is inside the lion's share of the samples*/
	int majorityClass = 0;
	int highestOfSingleClass = 0;
	for (int i = 0; i < numberOfPossibleAnswers; i++)
	{
		int numberOfTrue = 0;
		for (auto& sample : data->list)
		{
			if (sample->ConvertAnswerToInt() == i)
				numberOfTrue++;
		}
		if (highestOfSingleClass < numberOfTrue)
		{
			highestOfSingleClass = numberOfTrue;
			majorityClass = i;
		}
	}

	/*If there aren't any attribute left to split on, we put the answer as the majority class*/
	if (!attributeLeft)
	{
		node->splitData = -1;
		node->answer = Answer(majorityClass);
		return node;
	}

	/*Check how many spliting points there are for the specified feature*/
	int size = data->list[0]->features[attributeToSplitOn]->SplitingPoints();

	/*Create that ammount of children*/
	node->children = new Node*[size];
	node->numberOfChildren = size;
	for (int i = 0; i < size; i++)
		node->children[i] = nullptr;

	/*Split up the data */
	std::vector<VectorWrapper*> dataSplit(size);
	for (auto& split : dataSplit)
		split = new VectorWrapper;

	for (auto& sample : data->list)
	{
		dataSplit[sample->features[attributeToSplitOn]->SplitingGroupForSample()]->list.push_back(sample);
	}

	/*Mark this attribute as unavailable*/
	leftForSplit[attributeToSplitOn] = false;

	/* Go through all children and mark them as answer leaf or, if more spliting is needed,
	 * create a subtree.
	 */
	for (int i = 0; i < size; i++)
	{
		/*If there is data for this child*/
		if (dataSplit[i]->list.size())
		{
			int splitMajorityClass = 0;
			int splitHighestOfSingleClass = 0;

			/*Find the answer that the lion's share of the samples in one
			* split shares.
			*/
			for (int j = 0; j < numberOfPossibleAnswers; j++)
			{
				int numberOfTrue = 0;
				for (auto& answer : dataSplit[i]->list)
				{
					if (answer->ConvertAnswerToInt() == j)
						numberOfTrue++;
				}
				if (splitHighestOfSingleClass < numberOfTrue)
				{
					splitHighestOfSingleClass = numberOfTrue;
					splitMajorityClass = j;
				}
			}
			/*If all samples share answer, create the child should have this answer*/
			if (splitHighestOfSingleClass == dataSplit[i]->list.size())
			{
				node->children[i] = new Node;
				node->children[i]->featureForSplit = attributeToSplitOn;
				node->children[i]->answer = Answer(splitMajorityClass);
				node->children[i]->splitData = -1;
				node->children[i]->numberOfChildren = 0;
			}
			else
			{
				/*Check if a split can be done*/
				attributeLeft = false;
				for (auto& attributeToTest : leftForSplit)
				{
					if (attributeToTest == true)
					{
						attributeLeft = true;
						break;
					}
				}
				/*If we can: Split up and create a subtree*/
				if (attributeLeft)
				{
					node->children[i] = BuildRoot(dataSplit[i], leftForSplit);
				}

				/* Else mark this as an answer node and give the node the answer shared by the lion part of
				 * it's samples.
				 */
				else
				{
					node->children[i] = new Node;
					node->children[i]->featureForSplit = attributeToSplitOn;
					node->children[i]->answer = Answer(majorityClass);
					node->children[i]->splitData = -1;
					node->children[i]->numberOfChildren = 0;
				}
			}
		}
		/*
			*
			* If there is no data, mark this as an answer node and give the node the answer shared by the lion part of
			* it's samples.
			*
			*/
		else
		{
			node->children[i] = new Node;
			node->children[i]->answer = Answer(majorityClass);
			node->children[i]->featureForSplit = attributeToSplitOn;
			node->children[i]->splitData = -1;
			node->children[i]->numberOfChildren = 0;
		}
	}
	return node;
}

template <typename Answer>
typename DecisionTree<Answer>::Node* DecisionTree<Answer>::BuildRootForContiniousFeature(VectorWrapper* data,
	std::vector<bool> leftForSplit,
	int attributeToSplitOn)
{
	Node* node = new Node();

	std::vector<VectorWrapper*> dataSplit;

	/*Find the range of the continous "function"*/
	float max = -std::numeric_limits<float>::max();
	float min = std::numeric_limits<float>::max();

	for (auto& sample : data->list)
	{
		float value = sample->features[attributeToSplitOn]->ConvertToFloat();
		if (value < min)
			min = value;
		if (max < value)
			max = value;
	}

	/*Split up the data into smaller parts*/
	SplitContiniousFeature(data, attributeToSplitOn, min, max, dataSplit);

	/*Create one child for each data-set*/
	int size = dataSplit.size();
	node->featureForSplit = attributeToSplitOn;
	node->maxLimit = max;
	node->minLimit = min;
	node->children = new Node*[size];
	node->numberOfChildren = size;
	for (int i = 0; i < size; i++)
		node->children[i] = nullptr;

	/*Go through all children and assign them their future work.*/
	for (int i = 0; i < size; i++)
	{
		int majorityClass = 0;
		int highestOfSingleClass = 0;

		/*Find the answer that the lion's share of the samples in one
			* split shares.
			*/
		for (int j = 0; j < numberOfPossibleAnswers; j++)
		{
			int numberOfTrue = 0;
			for (auto& answer : dataSplit[i]->list)
			{
				if (answer->ConvertAnswerToInt() == j)
					numberOfTrue++;
			}
			if (highestOfSingleClass < numberOfTrue)
			{
				highestOfSingleClass = numberOfTrue;
				majorityClass = j;
			}
		}
		/*Find the range of the samples in the data split.*/
		float sampleMax = -std::numeric_limits<float>::max();
		float sampleMin = std::numeric_limits<float>::max();

		for (auto& answer : dataSplit[i]->list)
		{
			float value = answer->features[attributeToSplitOn]->ConvertToFloat();
			if (value < sampleMin)
				sampleMin = value;
			if (sampleMax < value)
				sampleMax = value;
		}
		/*
			* If only one answer is represented in the split,
			* mark this node as using that answer.
			*/
		if (dataSplit[i]->list.size() == highestOfSingleClass)
		{
			node->children[i] = new Node();
			node->children[i]->maxLimit = sampleMax;
			node->children[i]->minLimit = sampleMin;
			node->children[i]->featureForSplit = attributeToSplitOn;
			node->children[i]->answer = Answer(majorityClass);
			node->children[i]->splitData = -1;
			node->children[i]->numberOfChildren = 0;
		}
		/*If the size is above 50, recursiverly split it.*/
		else if (dataSplit[i]->list.size() > 50)
		{
			node->children[i] = BuildRootForContiniousFeature(dataSplit[i], leftForSplit, attributeToSplitOn);
			node->children[i]->maxLimit = sampleMax;
			node->children[i]->minLimit = sampleMin;
		}
		/*If there are more than 50 samples in the dataSplit,
			* the availability of a new splitting attribute is checked.
			* If one exist, BuildRoot() is called on to build a new sub-tree.
			* Otherwise, a child is created with answer equal to the highest possible answer.
			*/
		else if (dataSplit[i]->list.size())
		{
			leftForSplit[attributeToSplitOn] = false;
			bool attributeLeft = false;

			for (auto& attributeToTest : leftForSplit)
			{
				if (attributeToTest == true)
				{
					attributeLeft = true;
					break;
				}
			}
			if (attributeLeft)
			{
				node->children[i] = BuildRoot(dataSplit[i], leftForSplit);
				node->children[i]->maxLimit = sampleMax;
				node->children[i]->minLimit = sampleMin;
			}
			else
			{
				node->children[i] = new Node;
				node->children[i]->maxLimit = sampleMax;
				node->children[i]->minLimit = sampleMin;
				node->children[i]->featureForSplit = attributeToSplitOn;
				node->children[i]->answer = Answer(majorityClass);
				node->children[i]->splitData = -1;
				node->children[i]->numberOfChildren = 0;
			}
		}
		/*If there are no samples for this child, set the answer shared by the lion part of samples in the parent as answer.*/
		else
		{
			node->children[i] = new Node;
			node->children[i]->maxLimit = sampleMax;
			node->children[i]->minLimit = sampleMin;
			node->children[i]->featureForSplit = attributeToSplitOn;
			node->children[i]->answer = Answer(majorityClass);
			node->children[i]->splitData = -1;
			node->children[i]->numberOfChildren = 0;
		}
	}

	for (auto split : dataSplit)
		delete split;

	node->splitData = attributeToSplitOn;
	return node;
}
			