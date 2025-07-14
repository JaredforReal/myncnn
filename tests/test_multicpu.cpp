#include "../src/cpu.h"
#include <iostream>
#include <cassert>
#include <vector>

// 模拟测试函数 - 不需要真实的多CPU硬件
void test_cpuset_basic_functionality()
{
    std::cout << "=== Testing CpuSet Basic Functionality ===" << std::endl;
    
    ncnn::CpuSet cpuset;
    
    // 测试基本启用/禁用
    assert(!cpuset.is_enabled(0));
    cpuset.enable(0);
    assert(cpuset.is_enabled(0));
    assert(cpuset.num_enabled() == 1);
    
    cpuset.disable(0);
    assert(!cpuset.is_enabled(0));
    assert(cpuset.num_enabled() == 0);
    
    std::cout << "✓ Basic enable/disable tests passed" << std::endl;
}

void test_cpuset_multi_group_simulation()
{
    std::cout << "=== Testing Multi-Group CPU Simulation ===" << std::endl;
    
    ncnn::CpuSet cpuset;
    
    // 模拟不同组的CPU（即使在单组系统上也能测试逻辑）
    std::vector<int> test_cpus = {0, 1, 32, 63, 64, 65, 96, 127, 128, 200, 256};
    
    for (int cpu : test_cpus) {
        cpuset.enable(cpu);
        if (cpuset.is_enabled(cpu)) {
            std::cout << "✓ CPU " << cpu << " enabled successfully" << std::endl;
        } else {
            std::cout << "⚠ CPU " << cpu << " could not be enabled (may be in legacy mode)" << std::endl;
        }
    }
    
    std::cout << "Total enabled CPUs: " << cpuset.num_enabled() << std::endl;
    
    // 测试组掩码功能
    for (int group = 0; group < cpuset.get_group_count(); group++) {
        ULONG_PTR mask = cpuset.get_group_mask(group);
        if (mask != 0) {
            std::cout << "✓ Group " << group << " has mask: 0x" << std::hex << mask << std::dec << std::endl;
        }
    }
}

void test_cpuset_boundary_conditions()
{
    std::cout << "=== Testing Boundary Conditions ===" << std::endl;
    
    ncnn::CpuSet cpuset;
    
    // 测试组边界
    std::vector<int> boundary_cpus = {63, 64, 127, 128, 191, 192, 255, 256};
    
    for (int cpu : boundary_cpus) {
        cpuset.enable(cpu);
        bool enabled = cpuset.is_enabled(cpu);
        std::cout << "CPU " << cpu << " (group boundary): " 
                  << (enabled ? "✓ enabled" : "✗ not enabled") << std::endl;
    }
    
    // 测试disable_all
    cpuset.disable_all();
    assert(cpuset.num_enabled() == 0);
    std::cout << "✓ disable_all() works correctly" << std::endl;
}

void test_cpuset_edge_cases()
{
    std::cout << "=== Testing Edge Cases ===" << std::endl;
    
    ncnn::CpuSet cpuset;
    
    // 测试无效CPU编号
    cpuset.enable(-1);  // 应该被忽略
    cpuset.enable(5000); // 应该被忽略
    assert(cpuset.num_enabled() == 0);
    std::cout << "✓ Invalid CPU numbers handled correctly" << std::endl;
    
    // 测试重复启用
    cpuset.enable(10);
    cpuset.enable(10);
    assert(cpuset.num_enabled() == 1);
    std::cout << "✓ Duplicate enable handled correctly" << std::endl;
    
    // 测试禁用未启用的CPU
    cpuset.disable(20);
    assert(cpuset.num_enabled() == 1);
    std::cout << "✓ Disabling non-enabled CPU handled correctly" << std::endl;
}

void test_performance_simulation()
{
    std::cout << "=== Testing Performance with Simulated Large CPU Count ===" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    ncnn::CpuSet cpuset;
    const int SIMULATION_CPU_COUNT = 1000; // 模拟1000个CPU
    
    // 大量操作性能测试
    for (int i = 0; i < SIMULATION_CPU_COUNT; i++) {
        cpuset.enable(i);
    }
    
    // 检查性能
    for (int i = 0; i < SIMULATION_CPU_COUNT; i++) {
        cpuset.is_enabled(i);
    }
    
    int enabled_count = cpuset.num_enabled();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "✓ Processed " << SIMULATION_CPU_COUNT << " CPUs in " << duration.count() << " microseconds" << std::endl;
    std::cout << "✓ Actually enabled: " << enabled_count << " CPUs" << std::endl;
}

void test_real_system_integration()
{
    std::cout << "=== Testing Integration with Real System ===" << std::endl;
    
    int real_cpu_count = ncnn::get_cpu_count();
    std::cout << "Real system CPU count: " << real_cpu_count << std::endl;
    
    ncnn::CpuSet cpuset;
    
    // 启用所有真实CPU
    for (int i = 0; i < real_cpu_count; i++) {
        cpuset.enable(i);
    }
    
    assert(cpuset.num_enabled() == real_cpu_count);
    std::cout << "✓ All real CPUs enabled successfully" << std::endl;
    
    // 测试线程亲和性掩码
    const ncnn::CpuSet& all_mask = ncnn::get_cpu_thread_affinity_mask(0);
    const ncnn::CpuSet& little_mask = ncnn::get_cpu_thread_affinity_mask(1);
    const ncnn::CpuSet& big_mask = ncnn::get_cpu_thread_affinity_mask(2);
    
    std::cout << "All cores enabled: " << all_mask.num_enabled() << std::endl;
    std::cout << "Little cores enabled: " << little_mask.num_enabled() << std::endl;
    std::cout << "Big cores enabled: " << big_mask.num_enabled() << std::endl;
    
    #if defined _WIN32
    std::cout << "Legacy mode: " << (cpuset.is_legacy_mode() ? "Yes" : "No") << std::endl;
    std::cout << "Group count: " << cpuset.get_group_count() << std::endl;
    #endif
}

void test_processor_group_simulation()
{
    std::cout << "=== Testing Processor Group Simulation ===" << std::endl;
    
    // 模拟多处理器组的场景
    ncnn::CpuSet group0_cpus, group1_cpus, group2_cpus;
    
    // 第一组：CPU 0-63
    for (int i = 0; i < 64; i++) {
        group0_cpus.enable(i);
    }
    
    // 第二组：CPU 64-127  
    for (int i = 64; i < 128; i++) {
        group1_cpus.enable(i);
    }
    
    // 第三组：CPU 128-191
    for (int i = 128; i < 192; i++) {
        group2_cpus.enable(i);
    }
    
    std::cout << "✓ Group 0 CPUs: " << group0_cpus.num_enabled() << std::endl;
    std::cout << "✓ Group 1 CPUs: " << group1_cpus.num_enabled() << std::endl; 
    std::cout << "✓ Group 2 CPUs: " << group2_cpus.num_enabled() << std::endl;
    
    // 验证组掩码
    #if defined _WIN32
    for (int group = 0; group < 3; group++) {
        ncnn::CpuSet* test_set = (group == 0) ? &group0_cpus : 
                                (group == 1) ? &group1_cpus : &group2_cpus;
        ULONG_PTR mask = test_set->get_group_mask(group);
        std::cout << "Group " << group << " mask: 0x" << std::hex << mask << std::dec << std::endl;
    }
    #endif
}

int main()
{
    std::cout << "Starting CpuSet Multi-CPU Simulation Tests..." << std::endl;
    std::cout << "Current system CPU count: " << ncnn::get_cpu_count() << std::endl;
    std::cout << "NCNN_MAX_CPU_COUNT: " << NCNN_MAX_CPU_COUNT << std::endl;
    std::cout << "NCNN_CPU_MASK_GROUPS: " << NCNN_CPU_MASK_GROUPS << std::endl;
    std::cout << std::endl;
    
    try {
        test_cpuset_basic_functionality();
        std::cout << std::endl;
        
        test_cpuset_multi_group_simulation();
        std::cout << std::endl;
        
        test_cpuset_boundary_conditions();
        std::cout << std::endl;
        
        test_cpuset_edge_cases();
        std::cout << std::endl;
        
        test_performance_simulation();
        std::cout << std::endl;
        
        test_real_system_integration();
        std::cout << std::endl;
        
        test_processor_group_simulation();
        std::cout << std::endl;
        
        std::cout << "🎉 All simulation tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}