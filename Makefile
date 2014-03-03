all: beliefpropagationcpp

beliefpropagationcpp: BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc margAndScatt.o
	$(CXX) $(CXXFLAGS) -std=c++11 -o BeliefPropagation -I. BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc margAndScatt.o -lcudart -lcublas;
	rm margAndScatt.o

BeliefPropagationCpp/huginParser/*.cc: ./BeliefPropagationCpp/huginParser/HuginGrammar.jj
	cd ./BeliefPropagationCpp/huginParser ; \
	java -cp ../../javacc/bin/lib/javacc.jar javacc $(JAVACC_OPTS) HuginGrammar.jj ; \
	cd ../..

margAndScatt.o:
	nvcc -c -o margAndScatt.o BeliefPropagationCUDA/margAndScatt.cu -arch=compute_30
	
clean:
	/bin/rm -rf BeliefPropagation BeliefPropagationCpp/huginParser/*.cc BeliefPropagationCpp/huginParser/*.h
