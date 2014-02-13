all: beliefpropagationcpp

beliefpropagationcpp: BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc myLibKernelSmallN.o
	$(CXX) $(CXXFLAGS) -std=c++0x -o BeliefPropagation -I. BeliefPropagationCpp/*.cpp BeliefPropagationCpp/huginParser/*.cc BeliefPropagationCpp/CUDA/reduction.o -lcudart; \
	rm myLibKernelSmallN.o

BeliefPropagationCpp/huginParser/*.cc: ./BeliefPropagationCpp/huginParser/HuginGrammar.jj
	cd ./BeliefPropagationCpp/huginParser ; \
	java -cp ../../javacc/bin/lib/javacc.jar javacc $(JAVACC_OPTS) HuginGrammar.jj ; \
	cd ../..

myLibKernelSmallN.o:
	nvcc -c -o myLibKernelSmallN.o BeliefPropagationCUDA/myLibKernelSmallN.cu -arch=compute_30
	
clean:
	/bin/rm -rf BeliefPropagation BeliefPropagationCpp/huginParser/*.cc BeliefPropagationCpp/huginParser/*.h
