<script>
  import { onMount } from "svelte";
  import * as api from "api.js";
  import "bytemd/dist/index.min.css";
  import "../../../_utils.scss";
  import TagList from "../../../../components/_TagList.svelte";
  import { stores } from "@sapper/app";
import Swal from "sweetalert2";

  export let id;
  export let slug;
  export let reply_to_id;
  export let topics;
  export let user_replied;
  let value = "";

  let title = "";
  let taglist = [];
  let Viewer = null;
  let offset = 0;
  let limit = 50;
  let count = 0;
  let time = "";
  let votes = 0;
  let bytemd = null;
  let posted_by;
  let username;
  let image_url;
  let initials;
  let shown_ts;

  const { session } = stores();

  onMount(async () => {
    bytemd = await import("bytemd");
    Viewer = bytemd.Viewer;

    let response = await api.get(
      `t/${id}/${slug}`,
      localStorage.getItem("jwt")
    );

    if (response.topic) {
      title = response.title;
      value = response.topic.description;
      taglist = response.tags.map((tag) => tag.name);
      time = response.topic.created_at;
      votes = response.topic.votes;
      posted_by = response.topic.posted_by;
      username = response.topic.username;
      reply_to_id = posted_by;
      user_replied = username;
      image_url = response.topic.image_url;
      if (image_url === "") {
        initials = username[0];
      }
      let asked_ts = Date.parse(time);
      console.log(asked_ts);
      let now = Date.now();
      shown_ts = Math.floor((now - asked_ts) / 1000);
      if (shown_ts >= 259200) {
        asked_ts = new Date(time);
        let year = asked_ts.getYear() + 1900;
        shown_ts = asked_ts.getDay() + "/" + asked_ts.getMonth() + "/" + year;
      } else if (172800 <= shown_ts && shown_ts < 259200) {
        shown_ts = "2 days ago";
      } else if (86400 <= shown_ts && shown_ts < 172800) {
        shown_ts = "yesterday";
      } else if (3600 <= shown_ts && shown_ts < 8640000) {
        shown_ts = Math.floor(shown_ts / 3600) + " h";
      } else if (60 <= shown_ts && shown_ts < 3600) {
        console.log(shown_ts);
        shown_ts = Math.floor(shown_ts / 60) + " m";
      } else {
        shown_ts = shown_ts + " s";
      }
    }
    response = await api.get(
      `t/get-discussion/${id}/?time=${time}&limit=${limit}`,
      localStorage.getItem("jwt")
    );

    if (response.topics) {
      topics = response.topics;
      let now = Date.now();
      for (var i = 0; i < topics.length; i++) {
        if (topics[i].image_url === "") {
          topics[i].initials = response.topics[i].username[0];
        }
        let asked_ts = Date.parse(topics[i].created_at);
        let now = Date.now();
        let shown_ts = Math.floor((now - asked_ts) / 1000);
        if (shown_ts >= 259200) {
          asked_ts = new Date(topics[i].created_at);
          let year = asked_ts.getYear() + 1900;
          shown_ts = asked_ts.getDay() + "/" + asked_ts.getMonth() + "/" + year;
        } else if (172800 <= shown_ts && shown_ts < 259200) {
          shown_ts = "2 days ago";
        } else if (86400 <= shown_ts && shown_ts < 172800) {
          shown_ts = "yesterday";
        } else if (3600 <= shown_ts && shown_ts < 8640000) {
          shown_ts = Math.floor(shown_ts / 3600) + " h";
        } else if (60 <= shown_ts && shown_ts < 3600) {
          console.log(shown_ts);
          shown_ts = Math.floor(shown_ts / 60) + " m";
        } else {
          shown_ts = shown_ts + " s";
        }
        topics[i].shown_ts = shown_ts;
      }
      console.log(topics);
      offset += limit;
      if (response.topics.length) {
        time = response.topics[response.topics.length - 1].created_at;
      }
    }
  });
  function show_editor(reply_to, username) {
    reply_to_id = reply_to;
    user_replied = username;
    if (document.getElementById("editor").style.display === "none") {
      document.getElementById("editor").style.display = "block";
    } else {
      document.getElementById("editor").style.display = "none";
    }
    // var editorDiv = document.getElementById("container");
    // console.log(editorDiv.scrollHeight, editorDiv.scrollTop, editorDiv.clientHeight);
    // editorDiv.scrollTop = editorDiv.scrollHeight;
    // console.log(editorDiv.scrollHeight, editorDiv.scrollTop, editorDiv.clientHeight);
  }
  async function vote(vote, elementID) {
    if (!$session.user) {
      Swal.fire("You need to be logged in before voting.");
      return;
    }
    let data = {};
    data.vote = vote;
    data.id = elementID;
    const response = await api.post(
      "votes/",
      { data },
      localStorage.getItem("jwt")
    );

    if (response.error) {
      Swal.fire(response.error);
    } else {
      if (elementID == id) {
        votes = vote + parseInt(votes);
      } else {
        for (var i = 0; i < topics.length; i++) {
          if (topics[i].topic_id == elementID) {
            topics[i].votes = vote + parseInt(topics[i].votes);
            topics = topics;
            break
          }
        }
      }
    }

  }
</script>

<svelte:head>
  <title>{title}</title>
</svelte:head>
<div>
  <h3>{title}</h3>
  <hr />
  <div>
    <div style="float:left;margin-right:10px;z-index:2;">
      {#if image_url === ""}
        <a href="/user/{posted_by}/{username}">
          <p data-letters={initials.toUpperCase()} />
        </a>
      {:else}
        <a href="/user/{posted_by}/{username}">
          <img
            src={image_url}
            alt="profile pic"
            style="width: 3.5em;height: 3.5em;line-height: 3.5em;text-align: center;border-radius: 50%;"
          />
        </a>
      {/if}
      <br />
      <div style="text-align: center;font-size: 24px">
        {#if $session.user}
          <a href="/vote-up" class="upvote" on:click|preventDefault={vote(1, id)}>
            <i class="fas fa-angle-up" />
          </a>
        {/if}
        <br />
        <span style="text-align:center">{votes}</span>
        <br />
        {#if $session.user}
          <a href="/vote-down" class="downvote" on:click|preventDefault={vote(-1, id)}>
            <i class="fas fa-angle-down" />
          </a>
        {/if}
      </div>
    </div>
    <div style="float:left; position:relative;width:calc(100% - 70px)">
      <span style="font-weight:bold;color:#888">{username}</span>
      <span style="float:right">{shown_ts}</span>
      <svelte:component this={Viewer} {value} />
      <TagList {taglist} />
      {#if $session.user}
        <div style="float:right">
          <a
            href="/edit/{id}/{slug}"
            class="anchor"
            title="Edit your post"
            style="margin-right:5px"
            ><span class="material-icons" style="vertical-align:bottom"
              >edit</span
            > Edit</a
          >
          <a
            href="/report/{id}"
            class="anchor danger"
            title="Report abusive or inappropriate content"
            style="margin-right:5px"
            ><span class="material-icons" style="vertical-align:bottom"
              >report</span
            >Report</a
          >
          <a
            href="/share/{id}"
            class="anchor"
            title="Share a link to this post"
            style="margin-right:5px"
            ><span class="material-icons" style="vertical-align:bottom"
              >share</span
            >Share</a
          >
          <a
            href="/bookmark/{id}"
            class="anchor"
            title="Bookmark this post"
            style="margin-right:5px"
            ><span class="material-icons" style="vertical-align:bottom"
              >bookmark</span
            >Bookmark</a
          >
          <a
            href="/reply"
            on:click|preventDefault={show_editor(posted_by, username)}
            class="anchor"
            title="Reply to this post"
            style="margin-right:5px"
            ><span class="material-icons" style="vertical-align:bottom"
              >reply</span
            >Reply</a
          >
        </div>
      {/if}
    </div>
  </div>
  <div style="clear:both" />
  {#each topics as { topic_id, description, votes, posted_by, username, initials, image_url, shown_ts }}
    <hr style="border-bottom:1px solid;color:#eee" />
    <div>
      <div style="float:left;margin-right:10px">
        {#if image_url === "" || image_url === undefined}
          <a href="/user/{posted_by}/{username}">
            <p data-letters={initials.toUpperCase()} />
          </a>
        {:else}
          <a href="/user/{posted_by}/{username}">
            <img
              src={image_url}
              alt="profile pic"
              style="width: 3.5em;height: 3.5em;line-height: 3.5em;text-align: center;border-radius: 50%;"
            />
          </a>
        {/if}
        <br />
        <div style="text-align: center;font-size: 24px">
          {#if $session.user}
            <a href="/vote-up" class="upvote" on:click|preventDefault={vote(1, topic_id)}>
              <i class="fas fa-angle-up" />
            </a>
          {/if}
          <br />
          <span style="text-align:center">{votes}</span>
          <br />
          {#if $session.user}
<<<<<<< HEAD
          <a href="/vote-down" class="downvote" on:click|preventDefault={vote(-1, topic_id)}>
            <i class="fas fa-angle-down" />
          </a>
=======
            <a
              href="/vote-down"
              class="anchor"
              on:click|preventDefault={vote(-1, topic_id)}
            >
              <i class="fas fa-angle-down" />
            </a>
>>>>>>> parent of fdfa040... updated npm packages and working on accepting answers
          {/if}
        </div>
      </div>
      <div style="float:left; position:relative;width:calc(100% - 70px)">
        <span style="font-weight:bold;color:#888">{username}</span>
        <span style="float:right">{shown_ts}</span>
        <svelte:component this={Viewer} value={description} />
        {#if $session.user}
          <div style="float:right">
            <a
              href="/edit/{topic_id}/{slug}"
              class="anchor"
              title="Edit your post"
              style="margin-right:5px"
              ><span class="material-icons" style="vertical-align:bottom"
                >edit</span
              > Edit</a
            >
            <a
              href="/report/{topic_id}"
              class="anchor danger"
              title="Report abusive or inappropriate content"
              style="margin-right:5px"
              ><span class="material-icons" style="vertical-align:bottom"
                >report</span
              >Report</a
            >
            <a
              href="/reply"
              on:click|preventDefault={show_editor(posted_by, username)}
              class="anchor"
              title="Reply to this post"
              style="margin-right:5px"
              ><span class="material-icons" style="vertical-align:bottom"
                >reply</span
              >Reply</a
            >
            <a
              href="/share/{topic_id}"
              class="anchor"
              title="Share a link to this post"
              style="margin-right:5px"
              ><span class="material-icons" style="vertical-align:bottom"
                >share</span
              >Share</a
            >
            <a
              href="/bookmark/{topic_id}"
              class="anchor"
              title="Bookmark this post"
              style="margin-right:5px"
              ><span class="material-icons" style="vertical-align:bottom"
                >bookmark</span
              >Bookmark</a
            >
          </div>
        {/if}
      </div>
    </div>
    <div style="clear:both" />
  {/each}
</div>

<style>
  [data-letters]:before {
    content: attr(data-letters);
    display: inline-block;
    font-size: 1.5em;
    width: 2.5em;
    height: 2.5em;
    line-height: 2.5em;
    text-align: center;
    border-radius: 50%;
    background: #0f9d58;
    vertical-align: middle;
    color: white;
    margin-top: -10px;
  }
  p {
      font-weight: 300;
  }
</style>
