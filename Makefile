all: beliefpropagationcpp

beliefpropagationcpp: BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc sumKernelSmallN.o
	$(CXX) $(CXXFLAGS) -std=c++0x -o BeliefPropagation -I. BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc sumKernelSmallN.o -lcudart; \
	rm sumKernelSmallN.o

BeliefPropagationCpp/huginParser/*.cc: ./BeliefPropagationCpp/huginParser/HuginGrammar.jj
	cd ./BeliefPropagationCpp/huginParser ; \
	java -cp ../../javacc/bin/lib/javacc.jar javacc $(JAVACC_OPTS) HuginGrammar.jj ; \
	cd ../..

sumKernelSmallN.o:
	nvcc -c -o sumKernelSmallN.o BeliefPropagationCUDA/sumKernelSmallN.cu -arch=compute_30
	
clean:
	/bin/rm -rf BeliefPropagation BeliefPropagationCpp/huginParser/*.cc BeliefPropagationCpp/huginParser/*.h
