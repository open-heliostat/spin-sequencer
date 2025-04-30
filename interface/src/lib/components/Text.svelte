<script lang="ts">
    export let label: string;
    export let value: string;
    export let maxLength: number = 10000;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="text"
    class="input"
    maxlength={maxLength}
    id={label}
    bind:value={value}
    on:change={onChange}
/>