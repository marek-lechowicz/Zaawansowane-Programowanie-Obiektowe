//
// Created by Dominika on 01.11.2020.
//

#include "TSP.hpp"

#include <algorithm>
#include <stack>
#include <optional>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const CostMatrix& cm) {
    for (std::size_t r = 0; r < cm.size(); ++r) {
        for (std::size_t c = 0; c < cm.size(); ++c) {
            const auto& elem = cm[r][c];
            os << (is_inf(elem) ? "INF" : std::to_string(elem)) << " ";
        }
        os << "\n";
    }
    os << std::endl;

    return os;
}

/* PART 1 */

/**
 * Create path from unsorted path and last 2x2 cost matrix.
 * @return The vector of consecutive vertex.
 */
path_t StageState::get_path() {
    path_t path = {0};
    for (std::size_t i = 0; i<matrix_.size()-1;i++) {
        for (auto elem : unsorted_path_) {
            if (elem.row == *(path.end()-1)) {
                path.push_back(elem.col);
                break;
            }
        }
    }
//    for(std::size_t i = 0; i < matrix_.size(); i++) {
//        path[i] = path[i] + 1;
//    }
    return path;
}

/**
 * Get minimum values from each row and returns them.
 * @return Vector of minimum values in row.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_rows() const {
    std::vector<cost_t> vect_of_mins;
    int min_elem = 0;
    for (const auto& v : matrix_) {
        min_elem = *std::min_element(v.begin(), v.end());
        if(min_elem == INF) {
            min_elem = 0;
        }
        vect_of_mins.push_back(min_elem);
    }

    return vect_of_mins;
}


/**
 * Reduce rows so that in each row at least one zero value is present.
 * @return Sum of values reduced in rows.
 */
cost_t CostMatrix::reduce_rows() {
    std::vector<cost_t> vect_of_mins = get_min_values_in_rows();
    for (std::size_t row_number = 0; row_number < size(); row_number++) {
        for (std::size_t col_number = 0; col_number < size(); col_number++) {
            if (matrix_[row_number][col_number] != INF) {
                        matrix_[row_number][col_number] -= vect_of_mins[row_number];
                }
        }
    }
    cost_t sum = std::accumulate(vect_of_mins.begin(), vect_of_mins.end(), 0);
    return sum;
}


/**
 * Get minimum values from each column and returns them.
 * @return Vector of minimum values in columns.
 */
std::vector<cost_t> CostMatrix::get_min_values_in_cols() const {
    std::vector<cost_t> vect_of_mins;
    int min_elem = 0;
    for (std::size_t i = 0; i < matrix_.size(); i++) {
        std::vector<cost_t> new_vect = {};
        for (const auto& v : matrix_) {
            new_vect.push_back(v[i]);
        }
        min_elem = *std::min_element(new_vect.begin(), new_vect.end());
        if(min_elem == INF) {
            min_elem = 0;
        }
        vect_of_mins.push_back(min_elem);
    }


    return vect_of_mins;
}



/**
 * Reduces rows so that in each column at least one zero value is present.
 * @return Sum of values reduced in columns.
 */
cost_t CostMatrix::reduce_cols() {
    std::vector<cost_t> vect_of_mins = get_min_values_in_cols();
    for (std::size_t row_number = 0; row_number < size(); row_number++) {
        for (std::size_t col_number = 0; col_number < size(); col_number++) {
            if (matrix_[row_number][col_number] != INF) {
                matrix_[row_number][col_number] -= vect_of_mins[col_number];
            }
        }
    }
    cost_t sum = std::accumulate(vect_of_mins.begin(), vect_of_mins.end(), 0);
    return sum;
}

/**
 * Get the cost of not visiting the vertex_t (@see: get_new_vertex())
 * @param row
 * @param col
 * @return The sum of minimal values in row and col, excluding the intersection value.
 */
cost_t CostMatrix::get_vertex_cost(std::size_t row, std::size_t col) const {
    cost_t cost = 0;
    CostMatrix temp_matrix = matrix_;
    temp_matrix[row][col] = INF;
    cost = cost + temp_matrix.get_min_values_in_rows()[row];
    cost = cost + temp_matrix.get_min_values_in_cols()[col];
    return cost;
}

/* PART 2 */

/**
 * Choose next vertex to visit:
 * - Look for vertex_t (pair row and column) with value 0 in the current cost matrix.
 * - Get the vertex_t cost (calls get_vertex_cost()).
 * - Choose the vertex_t with maximum cost and returns it.
 * @param cm
 * @return The coordinates of the next vertex.
 */
NewVertex StageState::choose_new_vertex() {
    cost_t cost = 0;
    NewVertex vert;
    for (std::size_t row_number = 0; row_number<matrix_.size();row_number++){
        for (std::size_t col_number = 0; col_number<matrix_.size();col_number++){
            if (matrix_[row_number][col_number] == 0){
                if(matrix_.get_vertex_cost(row_number,col_number)>=cost){
                    vert.coordinates = vertex_t(row_number,col_number);
                    vert.cost = matrix_.get_vertex_cost(row_number,col_number);
                    cost = vert.cost;
                }
            }
        }
    }
    return vert;
}

/**
 * Update the cost matrix with the new vertex.
 * @param new_vertex
 */
void StageState::update_cost_matrix(vertex_t new_vertex) {
    // update lower bound
    update_lower_bound(reduce_cost_matrix());

    // remove selected row and col
    for(std::size_t row=0; row<matrix_.size(); row++) {
        for(std::size_t col=0; col<matrix_[row].size(); col++) {
            if(row == new_vertex.row) {
                matrix_[row][col] = INF;
            }
            if(col == new_vertex.col) {
                matrix_[row][col] = INF;
            }
            // forbid non-hamiltonian cycles
            for(auto el: unsorted_path_) {
                if(col == el.col) {
                    matrix_[row][col] = INF;
                }
            }
        }
    }

    // forbid reverse move
    matrix_[new_vertex.col][new_vertex.row] = INF;

}

/**
 * Reduce the cost matrix.
 * @return The sum of reduced values.
 */
cost_t StageState::reduce_cost_matrix() {
    cost_t reduced_rows = matrix_.reduce_rows();
    cost_t reduced_cols = matrix_.reduce_cols();
    return reduced_rows + reduced_cols;
}

/**
 * Given the optimal path, return the optimal cost.
 * @param optimal_path
 * @param m
 * @return Cost of the path.
 */
cost_t get_optimal_cost(const path_t& optimal_path, const cost_matrix_t& m) {
    cost_t cost = 0;

    for (std::size_t idx = 1; idx < optimal_path.size(); ++idx) {
        cost += m[optimal_path[idx - 1]][optimal_path[idx]];
    }

    // Add the cost of returning from the last city to the initial one.
    cost += m[optimal_path[optimal_path.size() - 1]][optimal_path[0]];

    return cost;
}

/**
 * Create the right branch matrix with the chosen vertex forbidden and the new lower bound.
 * @param m
 * @param v
 * @param lb
 * @return New branch.
 */
StageState create_right_branch_matrix(cost_matrix_t m, vertex_t v, cost_t lb) {
    CostMatrix cm(m);
    cm[v.row][v.col] = INF;
    return StageState(cm, {}, lb);
}

/**
 * Retain only optimal ones (from all possible ones).
 * @param solutions
 * @return Vector of optimal solutions.
 */
tsp_solutions_t filter_solutions(tsp_solutions_t solutions) {
    cost_t optimal_cost = INF;
    for (const auto& s : solutions) {
        optimal_cost = (s.lower_bound < optimal_cost) ? s.lower_bound : optimal_cost;
    }

    tsp_solutions_t optimal_solutions;
    std::copy_if(solutions.begin(), solutions.end(),
                 std::back_inserter(optimal_solutions),
                 [&optimal_cost](const tsp_solution_t& s) { return s.lower_bound == optimal_cost; }
    );

    return optimal_solutions;
}

/**
 * Solve the TSP.
 * @param cm The cost matrix.
 * @return A list of optimal solutions.
 */
tsp_solutions_t solve_tsp(const cost_matrix_t& cm) {

    StageState left_branch(cm);

    // The branch & bound tree.
    std::stack<StageState> tree_lifo;

    // The number of levels determines the number of steps before obtaining
    // a 2x2 matrix.

    //
    std::size_t n_levels = cm.size();

    tree_lifo.push(left_branch);   // Use the first cost matrix as the root.

    cost_t best_lb = INF;
    tsp_solutions_t solutions;

    while (!tree_lifo.empty()) {

        left_branch = tree_lifo.top();
        tree_lifo.pop();
        const cost_matrix_t current_cm = left_branch.get_matrix().get_matrix();

        while (left_branch.get_level() != n_levels && left_branch.get_lower_bound() <= best_lb) {
            // licz do końca komputerku, w żadne 2x2 się nie będziemy bawić

            // Repeat until a 2x2 matrix is obtained or the lower bound is too high...

                if (left_branch.get_level() == 0) {
                left_branch.reset_lower_bound();
            }

            // 1. Reduce the matrix in rows and columns.
            cost_t new_cost = 0; // @TODO (KROK 1)
            new_cost = left_branch.reduce_cost_matrix();

            // 2. Update the lower bound and check the break condition.
            left_branch.update_lower_bound(new_cost);
            if (left_branch.get_lower_bound() > best_lb) {
                break;
            }

            // 3. Get new vertex and the cost of not choosing it.
            NewVertex new_vertex = NewVertex(); // @TODO (KROK 2)
            new_vertex = left_branch.choose_new_vertex();


            // 4. @TODO Update the path - use append_to_path method.
            left_branch.append_to_path(new_vertex.coordinates);

            // 5. @TODO (KROK 3) Update the cost matrix of the left branch.
            left_branch.update_cost_matrix(new_vertex.coordinates);

            // 6. Update the right branch and push it to the LIFO.
            if(left_branch.get_level() <= n_levels - 2) {
                cost_t new_lower_bound = left_branch.get_lower_bound() + new_vertex.cost;
                tree_lifo.push(create_right_branch_matrix(current_cm, new_vertex.coordinates,
                                                          new_lower_bound));
            }
        }

        if (left_branch.get_lower_bound() <= best_lb) {
            // If the new solution is at least as good as the previous one,
            // save its lower bound and its path.
            best_lb = left_branch.get_lower_bound();
            path_t new_path = left_branch.get_path();
            path_t final_path = new_path;

            for (std::size_t i = 0; i < new_path.size() ;i++) {
                final_path[i] = new_path[i]+1;
            }

            solutions.push_back({get_optimal_cost(new_path, cm), final_path});
        }
    }

    return filter_solutions(solutions); // Filter solutions to find only optimal ones.
}

