//
// Created by lebon on 29/06/2022.
//

#include "test.h"
#include "../framework/data.h"
#include "../markers.h"
#include "../framework/dataStructures/graph.h"
#include "../framework/imageManager.h"
#include <opencv2/highgui/highgui.hpp>
#include <benchmark/benchmark.h>

static void BM_AddMarker1Cell2(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        state.ResumeTiming();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
    }
}

static void BM_AddMarker1Cell2MP(benchmark::State& state) {
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        state.ResumeTiming();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
    }
}

static void BM_AddMarker2Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        state.ResumeTiming();
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
    }
}

static void BM_AddMarker2Cell2MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        state.ResumeTiming();
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
    }
}

static void BM_RemoveMarker3Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        state.ResumeTiming();
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
    }
}

static void BM_RemoveMarker3Cell2MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        state.ResumeTiming();
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
    }
}

static void BM_RemoveMarker4Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
        state.ResumeTiming();
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,false);
    }
}

static void BM_RemoveMarker4Cell2MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
        state.ResumeTiming();
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,true);
    }
}

static void BM_AddMarker5Cell2(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,false);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,false);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,false);
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,false);
        state.ResumeTiming();
        testImg.addMarkers(markers5Cell2,nbMarkers5Cell2,false);
    }
}

static void BM_AddMarker5Cell2MP(benchmark::State& state){
    auto testim = cv::imread("cell2.jpg",cv::IMREAD_GRAYSCALE);

    for (auto _ : state) {
        state.PauseTiming();
        imageManager testImg = imageManager("cell2.jpg",testim);
        testImg.init();
        testImg.addMarkers(markers1Cell2,nbMarkers1Cell2,true);
        testImg.addMarkers(markers2Cell2,nbMarkers2Cell2,true);
        testImg.removeMarkers(markers3Cell2,nbMarkers3Cell2,true);
        testImg.removeMarkers(markers4Cell2,nbMarkers4Cell2,true);
        state.ResumeTiming();
        testImg.addMarkers(markers5Cell2,nbMarkers5Cell2,true);
    }
}

#define bench_num 100
// Register the function as a benchmark
BENCHMARK(BM_AddMarker1Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_AddMarker1Cell2MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_AddMarker2Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_AddMarker2Cell2MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_RemoveMarker3Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_RemoveMarker3Cell2MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_RemoveMarker4Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_RemoveMarker4Cell2MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_AddMarker5Cell2)->Unit(benchmark::kMillisecond)->Iterations(bench_num);
BENCHMARK(BM_AddMarker5Cell2MP)->Unit(benchmark::kMillisecond)->Iterations(bench_num);



BENCHMARK_MAIN();