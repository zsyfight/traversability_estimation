/*
 * TraversabilityEstimation.hpp
 *
 *  Created on: Oct 22, 2014
 *      Author: Ralf Kaestner, Peter Fankhauser
 *	 Institute: ETH Zurich, Autonomous Systems Lab
 */

#pragma once

// Grid Map
#include <grid_map/grid_map.hpp>

// Traversability estimation
#include "traversability_msgs/CheckFootprintPath.h"

// ROS
#include <ros/ros.h>
#include <nav_msgs/OccupancyGrid.h>
#include <tf/transform_listener.h>
#include <filters/filter_chain.h>

// STD
#include <vector>

// Eigen
#include <Eigen/Core>

namespace traversability_estimation {

  /*!
  * The terrain traversability estimation main class. Coordinates the ROS
  * interfaces, the timing, and the data handling between the other classes.
  */
  class TraversabilityEstimation {

  public:
    /*!
    * Constructor.
    * @param nodeHandle the ROS node handle.
    */
    TraversabilityEstimation(ros::NodeHandle& nodeHandle);

    /*!
    * Destructor.
    */
    virtual ~TraversabilityEstimation();

    /*!
     * Computes the traversability and add it as type to the elevation map.
     * Traversability is set between 0.0 and 1.0, where a value of 0.0 means not
     * traversable and 1.0 means fully traversable.
     * @param[in/out] elevationMap the map for which the traversability is computed.
     */
    bool computeTraversability(const grid_map::GridMap& elevationMap, grid_map::GridMap& traversabilityMap);

    /*!
     * ROS service callback function to return a boolean to indicate if a path is traversable.
     * @param request the ROS service request defining footprint path.
     * @param response the ROS service response containing the traversability of the footprint path.
     * @return true if successful.
     */
    bool checkFootprintPath(traversability_msgs::CheckFootprintPath::Request& request, traversability_msgs::CheckFootprintPath::Response& response);

  private:

    /*!
    * Reads and verifies the ROS parameters.
    * @return true if successful.
    */
    bool readParameters();

    /*!
    * Callback function for the update timer. Forces an update of the traversability
    * map from a new elevation map requested from the grid map service.
    * @param timerEvent the timer event.
    */
    void updateTimerCallback(const ros::TimerEvent& timerEvent);

    /*!
     * Gets the grid map for the desired submap center point.
     * @param[out] map the map that is received.
     * @return true if successful, false if ROS service call failed.
     */
    bool getGridMap(grid_map_msgs::GridMap& map);

    /*!
     * Gets the traversability value of the submap defined by the polygon. Is true if the
     * whole polygon is traversable.
     * @param[in] polygon polygon that defines submap of the traversability map.
     * @param[out] traversability traversability value of submap defined by the polygon, the traversability
     * is the mean of each cell within the polygon.
     * @return true if the whole polygon is traversable, false otherwise.
     */
    bool isTraversable(const grid_map::Polygon& polygon, double& traversability);

    //! ROS node handle.
    ros::NodeHandle& nodeHandle_;

    //! ROS service server.
    ros::ServiceServer footprintPathService_;

    //! Elevation map service client.
    ros::ServiceClient submapClient_;

    //! Name of the elevation submap service.
    std::string submapServiceName_;
  
    //! TF listener.
    tf::TransformListener transformListener_;

    //! Center point of the requested map.
    geometry_msgs::PointStamped submapPoint_;

    //! Id of the frame of the elevation map.
    std::string mapFrameId_;

    //! Id of the frame of the robot.
    std::string robotFrameId_;

    //! Publisher of the traversability occupancy grid.
    ros::Publisher traversabilityMapPublisher_;

    //! Publisher of the roughness filter occupancy grid.
    ros::Publisher footprintPolygonPublisher_;
  
    //! Timer for the map update.
    ros::Timer updateTimer_;
  
    //! Duration between map updates.
    ros::Duration updateDuration_;
    
    //! Requested map cell types.
    std::vector<std::string> requestedMapTypes_;

    //! Requested map length in [m].
    Eigen::Array2d mapLength_;

    //! Traversability map types.
    const std::string traversabilityType_;
    const std::string slopeType_;
    const std::string stepType_;
    const std::string roughnessType_;

    //! Filter Chain
    filters::FilterChain<grid_map::GridMap> filter_chain_;

    //! Traversability map.
    grid_map::GridMap traversabilityMap_;
  };

} /* namespace */