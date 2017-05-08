// Run: %dxc -T ps_6_0 -E main

void main() {
// CHECK-LABEL: %bb_entry = OpLabel
    bool c1, c2, c3, c4;
    int val = 0;

// CHECK:      [[c1:%\d+]] = OpLoad %bool %c1
// CHECK-NEXT: OpSelectionMerge %if_merge None
// CHECK-NEXT: OpBranchConditional [[c1]] %if_true %if_false
    if (c1) {
// CHECK-LABEL: %if_true = OpLabel
// CHECK-NEXT: [[c2:%\d+]] = OpLoad %bool %c2
// CHECK-NEXT: OpSelectionMerge %if_merge_0 None
// CHECK-NEXT: OpBranchConditional [[c2]] %if_true_0 %if_merge_0

// TODO: Move this basic block to the else branch
// CHECK-LABEL: %if_false = OpLabel
// CHECK-NEXT: [[c3:%\d+]] = OpLoad %bool %c3
// CHECK-NEXT: OpSelectionMerge %if_merge_1 None
// CHECK-NEXT: OpBranchConditional [[c3]] %if_true_1 %if_false_0

// TODO: Move this basic block to the end
// CHECK-LABEL: %if_merge = OpLabel
// CHECK-NEXT: OpReturn

        if (c2)
// CHECK-LABEL: %if_true_0 = OpLabel
// CHECK-NEXT: OpStore %val %int_1
// CHECK-NEXT: OpBranch %if_merge_0
            val = 1;

// CHECK-LABEL: %if_merge_0 = OpLabel
// CHECK-NEXT: OpBranch %if_merge
    } else {
        if (c3) {
// CHECK-LABEL: %if_true_1 = OpLabel
// CHECK-NEXT: OpStore %val %int_2
// CHECK-NEXT: OpBranch %if_merge_1
            val = 2;
        } else {
// CHECK-LABEL: %if_false_0 = OpLabel
// CHECK-NEXT: [[c4:%\d+]] = OpLoad %bool %c4
// CHECK-NEXT: OpSelectionMerge %if_merge_2 None
// CHECK-NEXT: OpBranchConditional [[c4]] %if_true_2 %if_merge_2

// TODO: Make this basic block the second to last one
// CHECK-LABEL: %if_merge_1 = OpLabel
// CHECK-NEXT: OpBranch %if_merge
            if (c4) {
// CHECK-LABEL: %if_true_2 = OpLabel
// CHECK-NEXT: OpStore %val %int_3
// CHECK-NEXT: OpBranch %if_merge_2
                val = 3;
            }
// CHECK-LABEL: %if_merge_2 = OpLabel
// CHECK-NEXT: OpBranch %if_merge_1
        }
    }
}
