# Zoom Meeting SDK for Linux Headless Bot Sample

This sample demonstrates how you can run the Zoom Meeting SDK for Linux within a Docker container and configure it for a
variety of use cases through an intuitive CLI or through a configuration file.

## Prerequisites

1. [Docker](https://www.docker.com/)
1. [Zoom Account](https://support.zoom.us/hc/en-us/articles/207278726-Plan-Types-)
1. [Zoom Meeting SDK Credentials](#config:-sdk-credentials) (Instructions below)
    1. SDK Key
    1. SDK Secret

### Clone the Repository

```bash
# Clone down this repository
git clone git@github.com:zoom/meetingsdk-headless-linux-sample.git
```

### 1. Download the Zoom Linux SDK

Download the latest version of the Zoom SDK for Linux from the Zoom Marketplace and place it in
the [lib/zoomsdk](lib/zoomsdk) folder of this repository.

### 2. Configure the Bot

If you don't already have them, follow the section on how
to [Get your Zoom Meeting SDK Credentials](#get-your-zoom-meeting-sdk-credentials).


#### Copy the sample config file

```bash
cp sample.config.ini config.ini
```

#### Fill out the config.ini

Here, you can set any of the CLI options so that the bot has them available when it runs. Start by adding your SDK Key
and SDK Secret in the relevant fields.

**At a minimum, you need to provide an SDK Key and SDK Secret along with information about the meeting you would like to join.**

You can either provide a Join URL, or a Meeting ID and Password.

### 3. Run the Bot

Run the Docker container in order to build and run the bot

```shell
docker compose up
```

### Get your Zoom Meeting SDK Credentials

In your web browser, navigate to [Zoom Developer Portal](https://developers.zoom.us/) and register/log into your
developer account.

Click the "Build App" button at the top and choose to "Meeting SDK" application.

1. Name your app
2. Choose whether to list your app on the marketplace or not
3. Click "Create"
4. Fill out the prerequisite information
5. Copy the SDK Key and SDK Secret to the config.ini file

For more information, you can follow [this guide](https://developers.zoom.us/docs/meeting-sdk/developer-accounts/)

### Keeping secrets secret

Remember, credentials should never be stored in a plaintext file for production use cases.

> :warning: **Never commit config.ini to version control:** The file likely contains Zoom SDK and Zoom OAuth
> Credentials

### Testing

At this time there are no tests.

## Need help?

If you're looking for help, try [Developer Support](https://devsupport.zoom.us) or
our [Developer Forum](https://devforum.zoom.us). Priority support is also available
with [Premier Developer Support](https://zoom.us/docs/en-us/developer-support-plans.html) plans.

### Documentation

Make sure to review [our documentation](https://developers.zoom.us/docs/meeting-sdk/linux/) as a reference when building
with the Zoom Meeting SDK for Linux.
